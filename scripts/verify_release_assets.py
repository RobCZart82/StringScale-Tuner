#!/usr/bin/env python3
"""Check both downloaded installer bundles before attaching release assets."""
import argparse
import json
from pathlib import Path
import re

from package_preview import digest

INSTALLER_SUFFIXES = {
    'windows-x64': '-setup.exe',
    'macos-arm64': '.pkg',
}


def verify_release_assets(directory, version, revision, build_number):
    if not re.fullmatch(r'[0-9]+\.[0-9]+\.[0-9]+(?:-[0-9a-z.-]+)?', version):
        raise ValueError('Expected a version such as 0.2.0-alpha.1')
    if not re.fullmatch(r'[0-9a-f]{40}', revision):
        raise ValueError('Expected the full source commit SHA')
    if not re.fullmatch(r'[1-9][0-9]*', build_number):
        raise ValueError('Expected a GitHub Actions build number')

    verified = []
    for platform, suffix in INSTALLER_SUFFIXES.items():
        stem = f'StringScaleTuner-{version}-{platform}-build{build_number}'
        installer = directory / (stem + suffix)
        checksum = installer.with_suffix(installer.suffix + '.sha256')
        manifest = installer.with_suffix(installer.suffix + '.json')
        actual_hash = digest(installer)
        if checksum.read_text(encoding='utf-8').split() != [actual_hash, installer.name]:
            raise ValueError(f'Checksum or filename mismatch: {installer.name}')
        metadata = json.loads(manifest.read_text(encoding='utf-8'))
        if not isinstance(metadata, dict):
            raise ValueError(f'{installer.name}: expected an installer manifest object')
        expected = {
            'application': 'StringScale Tuner',
            'version': version,
            'revision': revision,
            'platform': platform,
            'build_number': build_number,
            'installer': installer.name,
            'sha256': actual_hash,
            'payload_archive': stem + '.zip',
        }
        for key, value in expected.items():
            if metadata.get(key) != value:
                raise ValueError(f'{installer.name}: installer manifest mismatch for {key}')
        verified.extend((installer, checksum, manifest))
    return verified


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory', type=Path)
    parser.add_argument('--version', required=True)
    parser.add_argument('--revision', required=True)
    parser.add_argument('--build-number', required=True)
    args = parser.parse_args()
    try:
        assets = verify_release_assets(args.directory, args.version, args.revision, args.build_number)
    except (OSError, ValueError) as exc:
        parser.exit(1, f'Release asset verification failed: {exc}\n')
    for asset in assets:
        print(asset)


if __name__ == '__main__':
    main()
