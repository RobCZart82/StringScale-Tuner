from pathlib import Path
import sys
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parent.parent / 'scripts'))
from package_paths import validate_inventory_links


class PackageLinksTest(unittest.TestCase):
    def test_framework_links_and_parent_segments_stay_inside(self):
        validate_inventory_links([
            dict(path='App/Frameworks/QtCore.framework/Versions/Current', symlink='A'),
            dict(path='App/Frameworks/QtCore.framework/QtCore', symlink='Versions/Current/QtCore'),
            dict(path='App/Frameworks/QtCore.framework/Versions/A/QtCore'),
            dict(path='App/Resources/reference', symlink='../Frameworks/QtCore.framework/QtCore'),
        ])

    def test_rejects_external_targets_cycles_and_redirected_parents(self):
        cases = [
            [dict(path='licenses/link', symlink='/outside')],
            [dict(path='licenses/link', symlink='C:/outside')],
            [dict(path='licenses/link', symlink='..\\outside')],
            [dict(path='licenses/link', symlink='../../outside')],
            [dict(path='a', symlink='b'), dict(path='b', symlink='a')],
            [dict(path='a', symlink='a/file')],
            [dict(path='alias', symlink='real'),
             dict(path='alias/child', symlink='../../outside')],
            [dict(path='dir/up', symlink='..'),
             dict(path='alias', symlink='dir/up/../outside')],
            [dict(path='dir/up', symlink='..'), dict(path='dir/up/../outside')],
        ]
        for inventory in cases:
            with self.subTest(inventory=inventory), self.assertRaises(ValueError):
                validate_inventory_links(inventory)

    def test_installer_rejects_before_writing(self):
        from installer_payload import create_user_stage
        import tempfile
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            target = root / 'user'
            with self.assertRaises(ValueError):
                create_user_stage(root / 'stage', target,
                                  [dict(path='licenses/escape', symlink='/outside')], 'macos-arm64')
            self.assertFalse(target.exists())


if __name__ == '__main__':
    unittest.main()
