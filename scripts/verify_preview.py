#!/usr/bin/env python3
"""Verify the finished inner ZIP before it is uploaded, without extracting it."""
import argparse
import hashlib
import json
from pathlib import Path, PurePosixPath
import stat

from package_preview import digest
import zipfile


def relative_path(value):
    path = PurePosixPath(value)
    if not value or path.is_absolute() or '..' in path.parts or '\\' in value:
        raise ValueError(f'Invalid package path: {value}')
    return value


def verify(archive, expected_info, platform):
    checksum = archive.with_suffix('.zip.sha256').read_text(encoding='utf-8').split()
    if checksum != [digest(archive), archive.name]:
        raise ValueError('Archive SHA256 or checksum filename mismatch')
    root = archive.stem + '/'
    with zipfile.ZipFile(archive) as payload:
        names = payload.namelist()
        if len(names) != len(set(names)):
            raise ValueError('Duplicate archive paths')
        for name in names:
            relative_path(name)
            if not name.startswith(root) and not name.startswith('__MACOSX/'):
                raise ValueError(f'Unexpected archive root: {name}')
        info = json.loads(payload.read(root + 'build-info.json'))
        for key in ('application', 'version', 'revision', 'qt_version', 'system', 'preview'):
            if info[key] != expected_info[key]:
                raise ValueError(f'Build metadata mismatch: {key}')
        if info['platform'] != platform or info['revision'].endswith('-dirty'):
            raise ValueError('Wrong platform or uncommitted source changes')
        if archive.stem != f"StringScaleTuner-{info['version']}-{platform}-build{info['build_number']}":
            raise ValueError('Archive name does not identify this build')
        inventory = json.loads(payload.read(root + 'file-inventory.json'))
        listed = set()
        for item in inventory:
            name = root + relative_path(item['path'])
            if name in listed:
                raise ValueError(f'Duplicate inventory entry: {name}')
            listed.add(name)
            entry = payload.getinfo(name)
            if 'symlink' in item:
                if not stat.S_ISLNK(entry.external_attr >> 16):
                    raise ValueError(f'Symlink lost during packaging: {name}')
                if payload.read(name).decode('utf-8') != item['symlink']:
                    raise ValueError(f'Symlink target changed: {name}')
            else:
                if entry.file_size != item['bytes']:
                    raise ValueError(f'File size changed: {name}')
                value = hashlib.sha256()
                with payload.open(name) as stream:
                    for block in iter(lambda: stream.read(1024 * 1024), b''):
                        value.update(block)
                if value.hexdigest() != item['sha256']:
                    raise ValueError(f'File content changed: {name}')
        allowed = listed | {root + 'build-info.json', root + 'file-inventory.json'}
        for entry in payload.infolist():
            if not entry.is_dir() and entry.filename.startswith(root) and entry.filename not in allowed:
                raise ValueError(f'File missing from inventory: {entry.filename}')
        required = ['LICENSE', 'START-HERE-HU.md', 'TEST-RESULTS-HU.md',
                    'THIRD_PARTY_NOTICES.md', 'HARDWARE-TESTS-HU.md', 'RELEASE-NOTES-HU.md',
                    'manuals/StringScale-Tuner-User-Manual-HU.pdf',
                    'manuals/StringScale-Tuner-User-Manual-EN.pdf']
        if platform == 'windows-x64':
            required += ['bin/StringScaleTuner.exe', 'bin/Qt6Core.dll',
                         'bin/Qt6Multimedia.dll', 'plugins/platforms/qwindows.dll']
        else:
            required += ['StringScaleTuner.app/Contents/MacOS/StringScaleTuner',
                         'StringScaleTuner.app/Contents/Info.plist',
                         'StringScaleTuner.app/Contents/_CodeSignature/CodeResources']
            if info.get('signing_type') != 'ad-hoc':
                raise ValueError('Missing macOS signing metadata')
        for name in required:
            if root + name not in listed:
                raise ValueError(f'Required payload missing: {name}')
    print(f'Verified {archive.name}: SHA256, provenance, {len(listed)} inventory entries and required files')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--archive', type=Path, required=True)
    parser.add_argument('--expected-info', type=Path, required=True)
    parser.add_argument('--platform', choices=['windows-x64', 'macos-arm64'], required=True)
    args = parser.parse_args()
    verify(args.archive, json.loads(args.expected_info.read_text(encoding='utf-8')), args.platform)


if __name__ == '__main__':
    main()
