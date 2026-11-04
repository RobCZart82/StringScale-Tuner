#!/usr/bin/env python3
"""Regenerate committed PNG/ICO/ICNS assets from the vector master on macOS.
Requires a Qt SDK, Apple command-line tools, and Pillow. Not needed to build app.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile
from PIL import Image

root=Path(__file__).resolve().parent.parent
parser=argparse.ArgumentParser()
parser.add_argument('--qt-root',type=Path,required=True)
args=parser.parse_args()
frameworks=args.qt_root.resolve()/'lib'
icons=root/'resources/icons'
with tempfile.TemporaryDirectory(prefix='stringscale-icon-') as temp:
    temp=Path(temp)
    renderer=temp/'render-icon'
    subprocess.run(['c++','-std=c++20',str(root/'scripts/render_app_icon.cpp'),'-o',str(renderer),
                    '-F'+str(frameworks),'-framework','QtCore','-framework','QtGui','-framework','QtSvg',
                    '-Wl,-rpath,'+str(frameworks)],check=True)
    sizes=[16,20,24,32,40,48,64,128,256,512,1024]
    for size in sizes:
        subprocess.run([str(renderer),str(icons/'app-icon.svg'),str(icons/f'app-icon-{size}.png'),str(size)],check=True)
    images=[Image.open(icons/f'app-icon-{size}.png') for size in sizes if size<=256]
    Image.open(icons/'app-icon-1024.png').save(icons/'StringScaleTuner.ico',format='ICO',
        sizes=[(size,size) for size in sizes if size<=256],append_images=images)
    iconset=temp/'StringScaleTuner.iconset';iconset.mkdir()
    for size in [16,32,128,256,512]:
        for factor in [1,2]:
            suffix='@2x' if factor==2 else ''
            Image.open(icons/f'app-icon-{size*factor}.png').save(iconset/f'icon_{size}x{size}{suffix}.png')
    subprocess.run(['iconutil','-c','icns','-o',str(icons/'StringScaleTuner.icns'),str(iconset)],check=True)
print('Generated 16-1024px PNGs, multi-size ICO, and ICNS')
