#!/usr/bin/env python3
"""Prepare the checked source archives and Qt notices for the public preview."""
import argparse
import json
from pathlib import Path
import posixpath
import tarfile
from zipfile import ZIP_DEFLATED, ZIP_STORED, ZipFile

from package_preview import digest


def qt_notices(archives):
    lines = [
        'Qt 6.8.3 third-party notices',
        '',
        'Generated from the versioned Qt source archives listed in RUNTIME_SOURCES.json.',
        'The list includes upstream attributions for the source modules as a whole.',
        'Some entries relate to build tools or features outside the installed runtime.',
    ]
    for path in archives:
        if not path.name.startswith('qt'):
            continue
        records = []
        with tarfile.open(path, 'r|*') as archive:
            for member in archive:
                if member.isfile() and member.name.endswith('/qt_attribution.json'):
                    entries = json.loads(archive.extractfile(member).read(), strict=False)
                    for entry in entries if isinstance(entries, list) else [entries]:
                        files = entry.get('LicenseFiles', []) + [
                            entry[field] for field in ('LicenseFile', 'CopyrightFile')
                            if entry.get(field)
                        ]
                        locations = [posixpath.normpath(posixpath.join(
                            posixpath.dirname(member.name), name)) for name in files]
                        records.append((member.name, entry, list(dict.fromkeys(locations))))
        wanted = {name for _, _, locations in records for name in locations}
        texts = {}
        with tarfile.open(path, 'r|*') as archive:
            for member in archive:
                if member.isfile() and member.name in wanted:
                    texts[member.name] = archive.extractfile(member).read().decode('utf-8').rstrip()
        for record, entry, locations in sorted(records, key=lambda item: item[0]):
            lines.extend(('', '=' * 72, entry['Name'], f'Source record: {record}',
                          f"Licence: {entry.get('License', '')}"))
            for field in ('Version', 'Homepage', 'QtUsage', 'Copyright'):
                if entry.get(field):
                    lines.append(f'{field}: {entry[field]}')
            for location in locations:
                lines.extend(('', f'Upstream text: {location}', texts[location]))
    return '\n'.join(lines) + '\n'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source_directory', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--refresh-notices', action='store_true',
                        help='regenerate the checked-in Qt and FFmpeg notices from the pinned sources')
    args = parser.parse_args()
    root = Path(__file__).resolve().parent.parent
    licences = root / 'resources' / 'licenses'
    manifest = licences / 'RUNTIME_SOURCES.json'
    sources = json.loads(manifest.read_text(encoding='utf-8'))
    archives = []
    for record in sources:
        path = args.source_directory / record['file']
        if digest(path) != record['sha256']:
            raise ValueError(f'Source archive SHA256 mismatch: {path.name}')
        archives.append(path)

    ffmpeg = next(path for path in archives if path.name == 'FFmpeg-n7.1.tar.gz')
    with tarfile.open(ffmpeg) as archive:
        ffmpeg_notice = archive.extractfile('FFmpeg-n7.1/LICENSE.md').read()
    generated_notices = {
        'QT-THIRD-PARTY-NOTICES.txt': qt_notices(archives).encode('utf-8'),
        'FFmpeg-LICENSE.md': ffmpeg_notice,
    }
    for name, contents in generated_notices.items():
        notice = licences / name
        if args.refresh_notices:
            notice.write_bytes(contents)
        elif notice.read_bytes() != contents:
            raise ValueError(f'{name} differs from the pinned sources')

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with ZipFile(args.output, 'w', compression=ZIP_DEFLATED) as bundle:
        for path in archives:
            bundle.write(path, f'sources/{path.name}', compress_type=ZIP_STORED)
        for path in sorted(licences.iterdir()):
            bundle.write(path, f'licenses/{path.name}')
        bundle.write(root / 'docs/THIRD_PARTY_NOTICES.md', 'THIRD_PARTY_NOTICES.md')
    checksum = args.output.with_suffix(args.output.suffix + '.sha256')
    checksum.write_text(f'{digest(args.output)}  {args.output.name}\n', encoding='utf-8')
    print(args.output)
    print(checksum)


if __name__ == '__main__':
    main()
