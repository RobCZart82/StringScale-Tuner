#!/usr/bin/env python3
"""Verify the installed files and run GUI previews without opening audio input."""
import argparse
import json
import os
from pathlib import Path
import subprocess

from package_preview import digest

parser = argparse.ArgumentParser()
parser.add_argument('--root', type=Path, required=True)
parser.add_argument('--platform', choices=['windows-x64', 'macos-arm64'], required=True)
parser.add_argument('--expected-info', type=Path, required=True)
parser.add_argument('--preview-out', type=Path, required=True)
args = parser.parse_args()
mac = args.platform == 'macos-arm64'
documents = args.root / 'StringScale Tuner Documents' if mac else args.root
info = json.loads((documents / 'build-info.json').read_text(encoding='utf-8'))
expected = json.loads(args.expected_info.read_text(encoding='utf-8'))
for field in ['version', 'revision', 'qt_version']:
    if info[field] != expected[field]:
        raise ValueError(f'Installed build mismatch: {field}')
for record in json.loads((documents / 'file-inventory.json').read_text(encoding='utf-8')):
    name = record['path']
    target = args.root / name if not mac or name.startswith('StringScaleTuner.app/') else documents / name
    if 'symlink' in record:
        if not target.is_symlink() or os.readlink(target) != record['symlink']:
            raise ValueError(f'Installed symlink mismatch: {target}')
    elif not target.is_file() or digest(target) != record['sha256']:
        raise ValueError(f'Installed file mismatch: {target}')
if mac:
    bundle = args.root / 'StringScaleTuner.app'
    subprocess.run(['codesign', '--verify', '--deep', '--strict', str(bundle)], check=True)
    executable = bundle / 'Contents/MacOS/StringScaleTuner'
else:
    executable = args.root / 'bin/StringScaleTuner.exe'
# Do not let Qt SDK paths hide a missing deployed runtime dependency.
env = dict(os.environ)
for key in ['QT_QPA_PLATFORM', 'QT_PLUGIN_PATH', 'QT_QPA_PLATFORM_PLUGIN_PATH',
            'QML2_IMPORT_PATH', 'DYLD_FRAMEWORK_PATH', 'DYLD_LIBRARY_PATH', 'LD_LIBRARY_PATH']:
    env.pop(key, None)
qt_root = env.get('QT_ROOT_DIR', '')
if qt_root:
    env['PATH'] = os.pathsep.join(p for p in env.get('PATH', '').split(os.pathsep)
                                  if not p.lower().startswith(qt_root.lower()))
subprocess.run([str(executable.resolve()), '--render-preview', str(args.preview_out.resolve())],
               env=env, check=True, timeout=45)
print('Installed payload and isolated application preview passed')
