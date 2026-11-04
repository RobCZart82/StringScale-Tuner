#!/usr/bin/env python3
"""Package an already installed preview with provenance, a test sheet and SHA256."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


def digest(path):
    with path.open("rb") as stream:
        value = hashlib.sha256()
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
        return value.hexdigest()


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--stage", type=Path, required=True)
    parser.add_argument("--build-info", type=Path, required=True)
    parser.add_argument("--platform", choices=["windows-x64", "macos-arm64"], required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--qt-root", type=Path)
    args = parser.parse_args()
    stage = args.stage.resolve()
    info = json.loads(args.build_info.read_text(encoding="utf-8"))
    version = info["version"]
    if not version or any(c not in "0123456789abcdefghijklmnopqrstuvwxyz.-" for c in version):
        raise ValueError("Unsafe or missing version")
    executable = (stage / "bin/StringScaleTuner.exe" if args.platform == "windows-x64"
                  else stage / "StringScaleTuner.app/Contents/MacOS/StringScaleTuner")
    if not executable.is_file():
        raise FileNotFoundError(executable)
    # Deployment rewrites load paths and adds Qt libraries after linking, which
    # invalidates the linker's provisional signature. Seal the final bundle
    # before inventory/archive generation; do not claim Developer ID trust.
    if args.platform == "macos-arm64":
        bundle = stage / "StringScaleTuner.app"
        subprocess.run(["codesign", "--force", "--deep", "--sign", "-", str(bundle)], check=True)
        subprocess.run(["codesign", "--verify", "--deep", "--strict", str(bundle)], check=True)
        info["signing_type"] = "ad-hoc"
    root = Path(__file__).resolve().parent.parent
    for source, target in [("docs/PREVIEW_HU.md", "START-HERE-HU.md"),
                           ("docs/TEST_CHECKLIST_HU.md", "TEST-RESULTS-HU.md"),
                           ("docs/THIRD_PARTY_NOTICES.md", "THIRD_PARTY_NOTICES.md"),
                           ("docs/HARDWARE_TEST_RESULTS_HU.md", "HARDWARE-TESTS-HU.md"),
                           ("docs/RELEASE_NOTES_HU.md", "RELEASE-NOTES-HU.md")]:
        shutil.copy2(root / source, stage / target)
    run = os.environ.get("GITHUB_RUN_NUMBER", "local")
    if not run.isdigit() and run != "local":
        raise ValueError("Invalid build number")
    name = f"StringScaleTuner-{version}-{args.platform}-build{run}"
    info.update(platform=args.platform, build_number=run, publisher_signed=False, notarized=False)
    if os.environ.get("GITHUB_RUN_ID"):
        info["workflow_run_url"] = ("https://github.com/RobCZart82/StringScale-Tuner/actions/runs/"
                                    + os.environ["GITHUB_RUN_ID"])
    # SDK SBOMs help the release audit. They can include components not shipped.
    if args.qt_root and (args.qt_root / "sbom").is_dir():
        shutil.copytree(args.qt_root / "sbom", stage / "qt-sdk-sbom", dirs_exist_ok=True)
    inventory = []
    for path in sorted(stage.rglob("*")):
        relative = path.relative_to(stage).as_posix()
        if path.is_symlink():
            inventory.append(dict(path=relative, symlink=os.readlink(path)))
        elif path.is_file() and path.name not in ("build-info.json", "file-inventory.json"):
            inventory.append(dict(path=relative, bytes=path.stat().st_size, sha256=digest(path)))
    (stage / "file-inventory.json").write_text(json.dumps(inventory, indent=2)+"\n", encoding="utf-8")
    (stage / "build-info.json").write_text(json.dumps(info, indent=2)+"\n", encoding="utf-8")
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    archive = output / (name + ".zip")
    with tempfile.TemporaryDirectory(prefix=".package-", dir=output) as temp:
        payload = Path(temp) / name
        shutil.copytree(stage, payload, symlinks=True)
        if args.platform == "macos-arm64":
            subprocess.run(["ditto", "-c", "-k", "--sequesterRsrc", "--keepParent",
                            str(payload), str(archive)], check=True)
        else:
            shutil.make_archive(str(archive.with_suffix("")), "zip", temp, name)
    checksum = archive.with_suffix(".zip.sha256")
    checksum.write_text(f"{digest(archive)}  {archive.name}\n", encoding="utf-8")
    print(archive)
    if os.environ.get("GITHUB_OUTPUT"):
        with open(os.environ["GITHUB_OUTPUT"], "a", encoding="utf-8") as stream:
            stream.write(f"archive_path={archive}\nchecksum_path={checksum}\nartifact_name={name}\n")


if __name__ == "__main__":
    main()
