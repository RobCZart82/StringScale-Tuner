import hashlib
from pathlib import Path
import sys
import tempfile
import unittest

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'scripts'))
from installer_payload import create_user_stage, MANUALS


class InstallerPayloadTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.stage = Path(self.temp.name) / 'stage'
        self.user = Path(self.temp.name) / 'user'
        self.inventory = []
        for name in sorted(MANUALS | {'LICENSE', 'THIRD_PARTY_NOTICES.md',
                'manuals/README.md', 'TEST-RESULTS-HU.md', 'HARDWARE-TESTS-HU.md',
                'START-HERE-HU.md', 'RELEASE-NOTES-HU.md', 'qt-sdk-sbom/sdk.spdx',
                'licenses/vendor/COPYING'}):
            self.add(name)

    def add(self, name):
        source = self.stage / name
        source.parent.mkdir(parents=True, exist_ok=True)
        content = name.encode('utf-8')
        source.write_bytes(content)
        self.inventory.append(dict(path=name, bytes=len(content), sha256=hashlib.sha256(content).hexdigest()))

    def assert_clean(self, platform):
        records = create_user_stage(self.stage, self.user, self.inventory, platform)
        paths = {item['path'] for item in records}
        self.assertTrue(MANUALS <= paths)
        self.assertIn('README.txt', paths)
        self.assertIn('LICENSE', paths)
        self.assertIn('THIRD_PARTY_NOTICES.md', paths)
        self.assertIn('licenses/vendor/COPYING', paths)
        self.assertFalse(any(name.startswith('qt-sdk-sbom/') or name in {
            'manuals/README.md', 'TEST-RESULTS-HU.md', 'HARDWARE-TESTS-HU.md',
            'START-HERE-HU.md', 'RELEASE-NOTES-HU.md'} for name in paths))
        self.assertEqual(paths, {p.relative_to(self.user).as_posix() for p in self.user.rglob('*') if p.is_file()})
        return paths

    def test_windows_runtime_kept_and_documents_filtered(self):
        for name in ['bin/StringScaleTuner.exe', 'plugins/platforms/qwindows.dll', 'translations/qtbase_hu.qm']:
            self.add(name)
        paths = self.assert_clean('windows-x64')
        self.assertIn('plugins/platforms/qwindows.dll', paths)
        self.assertIn('translations/qtbase_hu.qm', paths)

    def test_mac_bundle_and_its_legal_resources_kept(self):
        self.add('StringScaleTuner.app/Contents/MacOS/StringScaleTuner')
        self.add('StringScaleTuner.app/Contents/Resources/third-party-NOTICE.txt')
        self.assertIn('StringScaleTuner.app/Contents/Resources/third-party-NOTICE.txt', self.assert_clean('macos-arm64'))

    def test_unknown_runtime_location_cannot_be_silently_dropped(self):
        self.add('new-runtime/runtime.dll')
        with self.assertRaisesRegex(ValueError, 'Unclassified'):
            create_user_stage(self.stage, self.user, self.inventory, 'windows-x64')

    def test_changed_source_cannot_enter_user_payload(self):
        (self.stage / 'LICENSE').write_bytes(b'changed')
        with self.assertRaisesRegex(ValueError, 'copy mismatch'):
            create_user_stage(self.stage, self.user, self.inventory, 'windows-x64')

    def test_missing_manual_fails(self):
        (self.stage / next(iter(MANUALS))).unlink()
        with self.assertRaises(FileNotFoundError):
            create_user_stage(self.stage, self.user, self.inventory, 'windows-x64')


if __name__ == '__main__':
    unittest.main()
