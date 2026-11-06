import json
import hashlib
from pathlib import Path
import sys
import tempfile
import unittest
sys.dont_write_bytecode=True
sys.path.insert(0,str(Path(__file__).resolve().parent.parent/'scripts'))
from runtime_licence_audit import create_audit

class LicenceAuditTest(unittest.TestCase):
    def test_committed_licence_texts_match_upstream_records(self):
        root=Path(__file__).resolve().parent.parent/'resources/licenses'
        records=json.loads((root/'SOURCES.json').read_text())
        self.assertEqual(len(records),3)
        for record in records:
            data=(root/record['file']).read_bytes()
            self.assertGreater(len(data),7000)
            self.assertEqual(hashlib.sha256(data).hexdigest(),record['sha256'])
            self.assertTrue(record['url'].startswith('https://raw.githubusercontent.com/'))

    def test_actual_binaries_only_with_unknowns_visible(self):
        with tempfile.TemporaryDirectory() as temp:
            root=Path(temp)
            paths={
                'bin/StringScaleTuner.exe':'application',
                'bin/Qt6Core.dll':'qt',
                'bin/avcodec-61.dll':'ffmpeg',
                'bin/vcruntime140.dll':'msvc-runtime',
                'bin/opengl32sw.dll':'mesa-llvmpipe',
                'bin/D3Dcompiler_47.dll':'microsoft-d3d-compiler',
                'bin/new-vendor.dll':'unclassified',
                'StringScaleTuner.app/Contents/Frameworks/QtCore.framework/Versions/A/QtCore':'qt',
            }
            for name in [*paths, 'notes.txt',
                         'StringScaleTuner.app/Contents/Frameworks/QtCore.framework/Versions/A/Resources/Info.plist',
                         'StringScaleTuner.app/Contents/Frameworks/QtCore.framework/Versions/A/_CodeSignature/CodeResources']:
                p=root/name;p.parent.mkdir(parents=True,exist_ok=True);p.write_bytes(b'fixture')
            report=create_audit(root,root/'audit.json')
            self.assertFalse(report['release_cleared'])
            self.assertTrue(report['pending'])
            self.assertEqual({p['path']:p['category'] for p in report['binaries']},paths)
            self.assertEqual(json.loads((root/'audit.json').read_text()),report)

if __name__=='__main__':unittest.main()
