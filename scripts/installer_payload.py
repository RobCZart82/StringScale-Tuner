"""Explicit user-installation policy; developer audit data stays outside the app."""
from pathlib import Path
import shutil

from package_preview import digest
from package_paths import relative_path, validate_inventory_links

MANUALS = {
    'manuals/StringScale-Tuner-User-Manual-HU.pdf',
    'manuals/StringScale-Tuner-User-Manual-EN.pdf',
}
USER_DOCUMENTS = MANUALS | {'LICENSE', 'THIRD_PARTY_NOTICES.md'}
DEVELOPER_FILES = {
    'START-HERE-HU.md', 'TEST-RESULTS-HU.md', 'HARDWARE-TESTS-HU.md',
    'RELEASE-NOTES-HU.md', 'manuals/README.md', 'runtime-licence-audit.json',
}


def is_runtime(name, platform):
    root = name.split('/')[0]
    return root == 'StringScaleTuner.app' if platform == 'macos-arm64' else root in {'bin', 'plugins', 'translations'}


def create_user_stage(stage, destination, inventory, platform):
    validate_inventory_links(inventory)
    selected = []
    for record in inventory:
        name = relative_path(record['path'])
        if is_runtime(name, platform) or name in USER_DOCUMENTS or name.startswith('licenses/'):
            source = stage / name
            target = destination / name
            target.parent.mkdir(parents=True, exist_ok=True)
            if 'symlink' in record:
                target.symlink_to(record['symlink'], target_is_directory=source.is_dir())
            else:
                shutil.copy2(source, target)
                if digest(target) != record['sha256']:
                    raise ValueError(f'User-stage copy mismatch: {name}')
            selected.append(record)
        elif name in DEVELOPER_FILES or name.startswith('qt-sdk-sbom/'):
            continue
        else:
            # A future runtime component must be classified, never silently lost.
            raise ValueError(f'Unclassified installer payload: {name}')
    for required in USER_DOCUMENTS:
        if not (destination / required).is_file():
            raise FileNotFoundError(required)
    readme = destination / 'README.txt'
    shutil.copy2(Path(__file__).resolve().parent.parent / 'docs/USER_README.txt', readme)
    selected.append(dict(path='README.txt', bytes=readme.stat().st_size, sha256=digest(readme)))
    return selected
