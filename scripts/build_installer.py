#!/usr/bin/env python3
"""Build native, unsigned test installers from an already verified preview stage."""
import argparse
import json
import os
from pathlib import Path
import plistlib
import shutil
import subprocess
import tempfile
import xml.etree.ElementTree as ET
import zipfile

from package_preview import digest
from verify_preview import verify


def build_mac(stage, destination, numeric_version):
    subprocess.run(['codesign', '--verify', '--deep', '--strict',
                    str(stage / 'StringScaleTuner.app')], check=True)
    with tempfile.TemporaryDirectory(prefix='stringscale-pkg-') as temp:
        work = Path(temp)
        payload = work / 'payload'
        applications = payload / 'Applications'
        applications.mkdir(parents=True)
        shutil.copytree(stage / 'StringScaleTuner.app', applications / 'StringScaleTuner.app', symlinks=True)
        documentation = applications / 'StringScale Tuner Documents'
        documentation.mkdir()
        for item in stage.iterdir():
            if item.name == 'StringScaleTuner.app':
                continue
            if item.is_dir():
                shutil.copytree(item, documentation / item.name, symlinks=True)
            else:
                shutil.copy2(item, documentation / item.name)
        components = work / 'components.plist'
        components.write_bytes(plistlib.dumps([dict(
            RootRelativeBundlePath='Applications/StringScaleTuner.app',
            BundleIsRelocatable=False, BundleIsVersionChecked=False,
            BundleHasStrictIdentifier=True, BundleOverwriteAction='upgrade')]))
        component = work / 'StringScaleTuner-component.pkg'
        subprocess.run(['pkgbuild', '--root', str(payload), '--install-location', '/',
                        '--identifier', 'org.robczart.stringscaletuner.pkg', '--version', numeric_version,
                        '--ownership', 'recommended', '--component-plist', str(components), str(component)], check=True)
        requirements = work / 'requirements.plist'
        requirements.write_bytes(plistlib.dumps(dict(arch=['arm64'], os=['12.0'])))
        distribution = work / 'Distribution.xml'
        subprocess.run(['productbuild', '--synthesize', '--product', str(requirements),
                        '--package', str(component), str(distribution)], check=True)
        tree = ET.parse(distribution)
        root = tree.getroot()
        title = root.find('title')
        if title is None:
            title = ET.SubElement(root, 'title')
        title.text = 'StringScale Tuner'
        domains = root.find('domains')
        if domains is None:
            domains = ET.SubElement(root, 'domains')
        domains.attrib.update(enable_localSystem='true', enable_currentUserHome='false', enable_anywhere='false')
        # The final product references a package beside this distribution file.
        for ref in root.findall('pkg-ref'):
            if ref.text and ref.text.strip():
                ref.text = component.name
        tree.write(distribution, encoding='utf-8', xml_declaration=True)
        subprocess.run(['productbuild', '--distribution', str(distribution),
                        '--package-path', str(work), str(destination)], check=True)
        # Validate the archived payload, not just the pre-archive staging folder.
        expanded = work / 'expanded'
        subprocess.run(['pkgutil', '--expand-full', str(destination), str(expanded)], check=True)
        installed = expanded / component.name / 'Payload' / 'Applications'
        bundle = installed / 'StringScaleTuner.app'
        subprocess.run(['codesign', '--verify', '--deep', '--strict', str(bundle)], check=True)
        for source in payload.rglob('*'):
            target = installed.parent / source.relative_to(payload)
            if source.is_symlink():
                if not target.is_symlink() or os.readlink(source) != os.readlink(target):
                    raise ValueError(f'PKG symlink mismatch: {source}')
            elif source.is_file() and (not target.is_file() or digest(source) != digest(target)):
                raise ValueError(f'PKG payload mismatch: {source}')


def build_windows(stage, destination, numeric_version, version, compiler):
    if compiler is None:
        compiler = shutil.which('ISCC.exe')
        if not compiler:
            for directory in [os.environ.get('ProgramFiles(x86)', ''), os.environ.get('ProgramFiles', '')]:
                candidate = Path(directory) / 'Inno Setup 6' / 'ISCC.exe'
                if candidate.is_file():
                    compiler = str(candidate)
                    break
    if not compiler:
        raise FileNotFoundError('Inno Setup 6 compiler (ISCC.exe) not found')
    definition = Path(__file__).resolve().parent.parent / 'packaging/windows/StringScaleTuner.iss'
    subprocess.run([str(compiler), f'/DStageDir={stage}', f'/DOutputDir={destination.parent}',
                    f'/DAppVersion={version}', f'/DNumericVersion={numeric_version}',
                    f'/DSetupName={destination.stem}', str(definition)], check=True)
    if not destination.is_file():
        raise FileNotFoundError(destination)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--stage', type=Path, required=True)
    parser.add_argument('--archive', type=Path, required=True)
    parser.add_argument('--build-info', type=Path, required=True)
    parser.add_argument('--platform', choices=['windows-x64', 'macos-arm64'], required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--iscc')
    args = parser.parse_args()
    stage = args.stage.resolve()
    expected = json.loads(args.build_info.read_text(encoding='utf-8'))
    verify(args.archive, expected, args.platform)
    info = json.loads((stage / 'build-info.json').read_text(encoding='utf-8'))
    if any(info[key] != expected[key] for key in ('version', 'revision', 'qt_version')):
        raise ValueError('Stage is not the verified build')
    with zipfile.ZipFile(args.archive) as archive:
        for metadata in ['build-info.json', 'file-inventory.json']:
            if (stage / metadata).read_bytes() != archive.read(args.archive.stem + '/' + metadata):
                raise ValueError(f'Stage metadata changed: {metadata}')
    # Recheck stage contents: installer compilers consume this directory, not ZIP.
    inventory = json.loads((stage / 'file-inventory.json').read_text(encoding='utf-8'))
    listed = {item['path'] for item in inventory} | {'build-info.json', 'file-inventory.json'}
    for path in stage.rglob('*'):
        if (path.is_file() or path.is_symlink()) and path.relative_to(stage).as_posix() not in listed:
            raise ValueError(f'Unverified extra stage file: {path}')
    for item in inventory:
        path = stage / item['path']
        if 'sha256' in item and digest(path) != item['sha256']:
            raise ValueError(f'Stage changed since ZIP creation: {path}')
        if 'symlink' in item and (not path.is_symlink() or os.readlink(path) != item['symlink']):
            raise ValueError(f'Stage symlink changed: {path}')
    output = args.output.resolve()
    output.mkdir(parents=True, exist_ok=True)
    suffix = '-setup.exe' if args.platform == 'windows-x64' else '.pkg'
    destination = output / (args.archive.stem + suffix)
    numeric_version = info['version'].split('-')[0]
    if args.platform == 'macos-arm64':
        build_mac(stage, destination, numeric_version)
    else:
        build_windows(stage, destination, numeric_version, info['version'], args.iscc)
    checksum = destination.with_suffix(destination.suffix + '.sha256')
    checksum.write_text(f'{digest(destination)}  {destination.name}\n', encoding='utf-8')
    manifest = destination.with_suffix(destination.suffix + '.json')
    manifest.write_text(json.dumps(dict(application=info['application'], version=info['version'],
        revision=info['revision'], platform=args.platform, build_number=info['build_number'],
        installer=destination.name, sha256=digest(destination), publisher_signed=False,
        notarized=False, payload_archive=args.archive.name), indent=2)+'\n', encoding='utf-8')
    print(destination)
    if os.environ.get('GITHUB_OUTPUT'):
        with open(os.environ['GITHUB_OUTPUT'], 'a', encoding='utf-8') as stream:
            stream.write(f'installer_path={destination}\ninstaller_checksum={checksum}\ninstaller_manifest={manifest}\n')


if __name__ == '__main__':
    main()
