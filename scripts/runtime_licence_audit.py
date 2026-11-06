"""Inventory actual shipped runtime binaries without claiming legal clearance."""
from pathlib import Path
import hashlib
import json


def category(name):
    lower = name.lower()
    leaf = Path(name).name.lower()
    if leaf == 'stringscaletuner' or leaf == 'stringscaletuner.exe':
        return 'application'
    if '.framework/' in lower and '/qt' in lower or leaf.startswith(('qt6', 'libq')) or lower.startswith('plugins/') or '/plugins/' in lower:
        return 'qt'
    if leaf.startswith(('avcodec', 'avformat', 'avutil', 'swresample', 'swscale', 'libav', 'libsw')):
        return 'ffmpeg'
    if leaf.startswith(('msvcp', 'vcruntime', 'concrt')):
        return 'msvc-runtime'
    if leaf == 'd3dcompiler_47.dll':
        return 'microsoft-d3d-compiler'
    if leaf == 'opengl32sw.dll':
        return 'mesa-llvmpipe'
    return 'unclassified'


def create_audit(stage, destination):
    binaries=[]
    for path in sorted(stage.rglob('*')):
        if path.is_symlink() or not path.is_file():
            continue
        name=path.relative_to(stage).as_posix()
        # Framework executables have no extension, but must not include Headers,
        # plist files, signatures or resource data as executable components.
        framework, separator, version_path = name.partition('.framework/Versions/')
        framework_binary = (separator and len(version_path.split('/')) == 2
                            and path.name == Path(framework).name)
        if path.suffix.lower() not in {'.dll', '.dylib', '.exe'} and not framework_binary and name != 'StringScaleTuner.app/Contents/MacOS/StringScaleTuner':
            continue
        binaries.append(dict(path=name, category=category(name),
                             sha256=hashlib.sha256(path.read_bytes()).hexdigest()))
    report=dict(schema_version=1, release_cleared=False, binaries=binaries,
                pending=['Qt bundled third-party notices and applicable licence mapping',
                         'Exact FFmpeg build configuration, version and corresponding source',
                         'Windows runtime/graphics redistribution terms where applicable',
                         'Final source-availability and redistribution review'])
    destination.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    return report
