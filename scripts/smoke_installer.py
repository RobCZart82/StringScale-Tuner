#!/usr/bin/env python3
"""Verify the installed files and run GUI previews without opening audio input."""
import argparse
import json
import os
import plistlib
from pathlib import Path
import subprocess

from package_preview import digest

parser = argparse.ArgumentParser()
parser.add_argument('--root', type=Path, required=True)
parser.add_argument('--platform', choices=['windows-x64', 'macos-arm64'], required=True)
parser.add_argument('--expected-info', type=Path, required=True)
parser.add_argument('--payload-inventory', type=Path, required=True)
parser.add_argument('--preview-out', type=Path, required=True)
args = parser.parse_args()
mac = args.platform == 'macos-arm64'
documents = args.root / 'StringScale Tuner Documents' if mac else args.root
audit = json.loads(args.payload_inventory.read_text(encoding='utf-8'))
info = audit['build_info']
expected = json.loads(args.expected_info.read_text(encoding='utf-8'))
for field in ['version', 'revision', 'qt_version']:
    if info[field] != expected[field]:
        raise ValueError(f'Installed build mismatch: {field}')
for record in audit['files']:
    name = record['path']
    target = args.root / name if not mac or name.startswith('StringScaleTuner.app/') else documents / name
    if 'symlink' in record:
        if not target.is_symlink() or os.readlink(target) != record['symlink']:
            raise ValueError(f'Installed symlink mismatch: {target}')
    elif not target.is_file() or digest(target) != record['sha256']:
        raise ValueError(f'Installed file mismatch: {target}')
# Fresh installations must not contain test sheets, SDK SBOMs or build metadata.
expected_paths = {record['path'] for record in audit['files']}
if mac:
    actual_documents = {path.relative_to(documents).as_posix() for path in documents.rglob('*')
                        if path.is_file() or path.is_symlink()}
    expected_documents = {name for name in expected_paths if not name.startswith('StringScaleTuner.app/')}
    if actual_documents != expected_documents:
        raise ValueError(f'Unexpected installed documents: {actual_documents ^ expected_documents}')
else:
    actual_paths = {path.relative_to(args.root).as_posix() for path in args.root.rglob('*')
                    if path.is_file() or path.is_symlink()}
    generated_uninstaller = {'unins000.exe', 'unins000.dat', 'unins000.msg'}
    if actual_paths - generated_uninstaller != expected_paths:
        raise ValueError(f'Unexpected installed payload: {(actual_paths - generated_uninstaller) ^ expected_paths}')
if mac:
    bundle = args.root / 'StringScaleTuner.app'
    subprocess.run(['codesign', '--verify', '--deep', '--strict', str(bundle)], check=True)
    bundle_info = plistlib.loads((bundle / 'Contents/Info.plist').read_bytes())
    icon = bundle / 'Contents/Resources' / bundle_info['CFBundleIconFile']
    if not icon.is_file() or icon.read_bytes()[:4] != b'icns':
        raise ValueError('Missing or invalid installed macOS app icon')
    executable = bundle / 'Contents/MacOS/StringScaleTuner'
else:
    executable = args.root / 'bin/StringScaleTuner.exe'
    import ctypes
    extractor = ctypes.WinDLL('shell32').ExtractIconExW
    extractor.argtypes = [ctypes.c_wchar_p, ctypes.c_int, ctypes.c_void_p, ctypes.c_void_p, ctypes.c_uint]
    extractor.restype = ctypes.c_uint
    if extractor(str(executable.resolve()), -1, None, None, 0) == 0:
        raise ValueError('Windows executable has no native icon resource')
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
