#include "about_dialog.hpp"
#include "ui_language.hpp"
#include <QCoreApplication>
#include <QEvent>
#include <QDialogButtonBox>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QSvgRenderer>
#include <QVBoxLayout>

namespace {
class BrandImage : public QWidget {
public:
    BrandImage(bool signature, QWidget* parent) : QWidget(parent), signature_(signature) {
        setMinimumHeight(signature ? 110 : 160);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setAccessibleName(signature ? "Gyuricza Róbert" : "GYR");
    }
protected:
    void paintEvent(QPaintEvent*) override {
        const bool dark = palette().color(QPalette::Window).lightness() < 128;
        const QString path = signature_
            ? (dark ? ":/branding/Alairas-Vektor-light.svg" : ":/branding/Alairas-Vektor.svg")
            : (dark ? ":/branding/GYR-light.svg" : ":/branding/GYR-black.svg");
        if (path != source_) { renderer_.load(path); source_ = path; }
        QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
        const QRectF available = QRectF(rect()).adjusted(20,12,-20,-12);
        // The SVG renderer maps its original nonzero viewBox into this aspect-preserving rectangle.
        const QSizeF size = renderer_.viewBoxF().size().scaled(available.size(), Qt::KeepAspectRatio);
        renderer_.render(&p, QRectF(available.center()-QPointF(size.width()/2,size.height()/2),size));
    }
private:
    bool signature_;
    QString source_;
    QSvgRenderer renderer_;
};
}
AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent) {
    setObjectName("aboutDialog");
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24,20,24,20); layout->setSpacing(10);
    auto* title = new QLabel("StringScale Tuner");
    auto font=title->font();font.setPointSize(23);font.setBold(true);title->setFont(font);
    title->setAlignment(Qt::AlignCenter);layout->addWidget(title);
    auto* version=new QLabel;version->setAlignment(Qt::AlignCenter);layout->addWidget(version);
    auto* description=new QLabel;description->setAlignment(Qt::AlignCenter);description->setWordWrap(true);layout->addWidget(description);
    layout->addWidget(new BrandImage(false,this),2);
    auto* author=new QLabel;author->setAlignment(Qt::AlignCenter);layout->addWidget(author);
    auto* name=new QLabel("Gyuricza Róbert · RobCZart82");name->setAlignment(Qt::AlignCenter);layout->addWidget(name);
    layout->addWidget(new BrandImage(true,this),1);
    auto* project=new QLabel("<a href=\"https://github.com/RobCZart82/StringScale-Tuner\">GitHub · StringScale Tuner</a>");
    project->setObjectName("projectLink");project->setAlignment(Qt::AlignCenter);project->setOpenExternalLinks(true);layout->addWidget(project);refreshProjectLink();
    auto* buttons=new QDialogButtonBox(QDialogButtonBox::Close);layout->addWidget(buttons);
    connect(buttons,&QDialogButtonBox::rejected,this,&QDialog::close);
    const auto translate=[this,version,description,author,buttons] {
        setWindowTitle(uiText("Névjegy","About"));
        version->setText(uiText("Verzió: %1","Version: %1").arg(QCoreApplication::applicationVersion()));
        description->setText(uiText("Ingyenes gitár- és basszushangoló","Free guitar and bass tuner"));
        author->setText(uiText("Készítette","Created by"));
        buttons->button(QDialogButtonBox::Close)->setText(uiText("Bezárás","Close"));
    };
    connect(&UiLanguage::instance(),&UiLanguage::changed,this,translate);translate();
    setMinimumSize(440,560);resize(500,620);
}

void AboutDialog::refreshProjectLink() {
    if(auto* link=findChild<QLabel*>("projectLink"))
        link->setText(QString("<a style=\"color:%1\" href=\"https://github.com/RobCZart82/StringScale-Tuner\">GitHub · StringScale Tuner</a>")
            .arg(palette().color(QPalette::Highlight).name()));
}
void AboutDialog::changeEvent(QEvent* event) {
    QDialog::changeEvent(event);
    if(event->type()==QEvent::PaletteChange)refreshProjectLink();
}
