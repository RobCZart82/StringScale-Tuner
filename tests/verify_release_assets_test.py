import json
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'scripts'))
from package_preview import digest
from verify_release_assets import verify_release_assets


class VerifyReleaseAssetsTest(unittest.TestCase):
    def test_release_channels(self):
        for version in ('0.2.0-alpha.1', '1.0.0'):
            with self.subTest(version=version):
                self.check_release_assets(version)

    def check_release_assets(self, version):
        revision, build = 'a' * 40, '77'
        is_preview = '-' in version
        with tempfile.TemporaryDirectory() as tmp:
            directory = Path(tmp)
            for platform, suffix in [('windows-x64', '-setup.exe'), ('macos-arm64', '.pkg')]:
                stem = f'StringScaleTuner-{version}-{platform}-build{build}'
                installer = directory / (stem + suffix)
                installer.write_bytes(platform.encode())
                sha = digest(installer)
                checksum = directory / (installer.name + '.sha256')
                checksum.write_text(f'{sha}  {installer.name}\n')
                manifest = directory / (installer.name + '.json')
                data = dict(application='StringScale Tuner', version=version, revision=revision,
                            platform=platform, build_number=build, installer=installer.name,
                            sha256=sha, payload_archive=stem + '.zip')
                if not is_preview:
                    data['preview'] = False
                manifest.write_text(json.dumps(data))

            self.assertEqual(len(verify_release_assets(directory, version, revision, build)), 6)
            for bad_preview in (not is_preview, int(is_preview), None):
                data['preview'] = bad_preview
                manifest.write_text(json.dumps(data))
                with self.assertRaisesRegex(ValueError, 'mismatch for preview'):
                    verify_release_assets(directory, version, revision, build)
            if not is_preview:
                del data['preview']
                manifest.write_text(json.dumps(data))
                with self.assertRaisesRegex(ValueError, 'mismatch for preview'):
                    verify_release_assets(directory, version, revision, build)
            data['preview'] = is_preview
            data['revision'] = 'b' * 40
            manifest.write_text(json.dumps(data))
            with self.assertRaisesRegex(ValueError, 'mismatch for revision'):
                verify_release_assets(directory, version, revision, build)
            data['revision'] = revision
            manifest.write_text(json.dumps(data))
            installer.write_bytes(b'changed after download')
            with self.assertRaisesRegex(ValueError, 'Checksum'):
                verify_release_assets(directory, version, revision, build)
