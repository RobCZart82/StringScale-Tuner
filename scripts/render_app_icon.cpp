// One-off vector rasterizer for native icon assets. No microphone or GUI window.
#if defined(__aarch64__)
#include <arm_acle.h>
#endif
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtSvg/QSvgRenderer>

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    if(argc != 4) return 1;
    QFile source(QString::fromLocal8Bit(argv[1]));
    if(!source.open(QIODevice::ReadOnly)) return 2;
    auto svg=source.readAll();
    const int size=QString::fromLocal8Bit(argv[3]).toInt();
    if(size<16 || size>1024) return 3;
    if(size<=32) {
        // Optical adjustment: keep the needle and arc clear at taskbar sizes.
        svg.replace("stroke-width=\"18\"", "stroke-width=\"40\"");
        svg.replace("stroke-width=\"22\"", "stroke-width=\"48\"");
        svg.replace("id=\"fineTicks\"", "id=\"fineTicks\" display=\"none\"");
    }
    QSvgRenderer renderer(svg);
    if(!renderer.isValid()) return 4;
    QImage image(size,size,QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);painter.setRenderHint(QPainter::Antialiasing);
    renderer.render(&painter);painter.end();
    return image.save(QString::fromLocal8Bit(argv[2]))?0:5;
}
