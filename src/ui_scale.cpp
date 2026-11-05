#include "ui_scale.hpp"
#include <QAbstractButton>
#include <QDialog>
#include <QFont>
#include <QFormLayout>
#include <QLayout>
#include <QRegularExpression>
#include <QSettings>
#include <QVariant>
#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#include <algorithm>

namespace {
int validPercent(int value) {
    return value==75 || value==100 || value==125 || value==150 ? value : 125;
}
bool belongsTo(QWidget* widget, QWidget* root) {
    for(auto* parent=widget; parent && parent!=root; parent=parent->parentWidget())
        if(qobject_cast<QDialog*>(parent)) return false;
    return true;
}
void remember(QObject* object, const char* key, const QVariant& value) {
    if(!object->property(key).isValid()) object->setProperty(key,value);
}
}
UiScale& UiScale::instance() { static UiScale scale;return scale; }
int UiScale::savedPercent() { return validPercent(QSettings().value("appearance/scalePercent",125).toInt()); }
int UiScale::fitPercent(int requested, QSize available) {
    requested=validPercent(requested);
    for(int preset : {150,125,100,75})
        if(preset<=requested && 1000*preset/125<=available.width() &&
           850*preset/125<=available.height()) return preset;
    return 75;
}
QSize UiScale::fitWindowSize(int percent, QSize available) {
    return fitWindowSize(QSize(1000,850),percent,available);
}
QSize UiScale::fitWindowSize(QSize preferred, int percent, QSize available) {
    percent=validPercent(percent);
    const double scale=percent/125.0;
    return QSize(qRound(preferred.width()*scale),qRound(preferred.height()*scale))
        .boundedTo(available.expandedTo(QSize(1,1)));
}
int UiScale::px(int value) const { return value>0 ? std::max(1,qRound(value*factor())) : value; }
QString UiScale::styleSheet(const QString& base) const {
    static const QRegularExpression pixels("(\\d+)px");
    QString result;qsizetype end=0;
    auto matches=pixels.globalMatch(base);
    while(matches.hasNext()) {
        const auto match=matches.next();
        result+=base.mid(end,match.capturedStart()-end)+QString::number(px(match.captured(1).toInt()))+"px";
        end=match.capturedEnd();
    }
    return result+base.mid(end);
}
void UiScale::setPercent(int percent, bool persist) {
    percent=validPercent(percent);
    if(persist)QSettings().setValue("appearance/scalePercent",percent);
    if(percent_==percent)return;
    percent_=percent;emit changed(percent);
}
void UiScale::prepare(QWidget* root, QSize preferred, bool fitToScreen) {
    root->setProperty("ssPreferredSize",preferred);
    const bool dialog=qobject_cast<QDialog*>(root)!=nullptr;
    root->setProperty("ssFitToScreen",dialog && fitToScreen);
    if(dialog) {
        auto* content=new QWidget;
        content->setFixedSize(preferred);
        // QWidget transfers an existing layout and its children to this canvas.
        content->setLayout(root->layout());
        auto* scroll=new QScrollArea(root);
        scroll->setObjectName("dialogScrollArea");
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setWidget(content);
        auto* outer=new QVBoxLayout(root);
        outer->setContentsMargins(0,0,0,0);outer->setSpacing(0);
        outer->addWidget(scroll);
        if(fitToScreen) {
            // Create the native handle before subscribing to screen changes.
            root->winId();
            connect(root->windowHandle(),&QWindow::screenChanged,root,[this,root]{apply(root);});
            const auto watchScreen=[this,root](QScreen* screen) {
                connect(screen,&QScreen::availableGeometryChanged,root,[this,root]{apply(root);});
            };
            for(auto* screen:QGuiApplication::screens())watchScreen(screen);
            connect(qGuiApp,&QGuiApplication::screenAdded,root,watchScreen);
        }
    }
    connect(this,&UiScale::changed,root,[this,root]{apply(root);});
    apply(root);
}
void UiScale::apply(QWidget* root) {
    QList<QWidget*> widgets{root};
    for(auto* widget:root->findChildren<QWidget*>())
        if(belongsTo(widget,root)) widgets.append(widget);
    // Capture the complete baseline before any parent font propagation.
    for(auto* widget:widgets) {
        remember(widget,"ssBaseFont",widget->font());
        remember(widget,"ssBaseMinimum",widget->minimumSize());
        remember(widget,"ssBaseMaximum",widget->maximumSize());
        if(auto* button=qobject_cast<QAbstractButton*>(widget))
            remember(button,"ssBaseIconSize",button->iconSize());
    }
    const auto scaled=[this](QSize size) {
        return QSize(size.width()>=QWIDGETSIZE_MAX?QWIDGETSIZE_MAX:px(size.width()),
                     size.height()>=QWIDGETSIZE_MAX?QWIDGETSIZE_MAX:px(size.height()));
    };
    for(auto* widget:widgets) {
        auto font=qvariant_cast<QFont>(widget->property("ssBaseFont"));
        if(font.pointSizeF()>0) font.setPointSizeF(font.pointSizeF()*factor());
        else if(font.pixelSize()>0)font.setPixelSize(px(font.pixelSize()));
        widget->setFont(font);
        widget->setMinimumSize(scaled(widget->property("ssBaseMinimum").toSize()));
        widget->setMaximumSize(scaled(widget->property("ssBaseMaximum").toSize()));
        if(auto* button=qobject_cast<QAbstractButton*>(widget))
            button->setIconSize(scaled(button->property("ssBaseIconSize").toSize()));
    }
    for(auto* layout:root->findChildren<QLayout*>()) {
        if(!belongsTo(layout->parentWidget(),root))continue;
        const auto margins=layout->contentsMargins();
        remember(layout,"ssBaseMargins",QVariantList{margins.left(),margins.top(),margins.right(),margins.bottom()});
        remember(layout,"ssBaseSpacing",layout->spacing());
        const auto base=layout->property("ssBaseMargins").toList();
        layout->setContentsMargins(px(base[0].toInt()),px(base[1].toInt()),px(base[2].toInt()),px(base[3].toInt()));
        layout->setSpacing(px(layout->property("ssBaseSpacing").toInt()));
    }
    QSize size=scaled(root->property("ssPreferredSize").toSize());
    if(root->property("ssFitToScreen").toBool() && root->screen()) {
        const auto margins=root->windowHandle()?root->windowHandle()->frameMargins():QMargins(8,32,8,8);
        const QSize available=root->screen()->availableGeometry().size()-QSize(
            margins.left()+margins.right()+8,std::max(32,margins.top()+margins.bottom())+8);
        size=fitWindowSize(root->property("ssPreferredSize").toSize(),percent_,available);
    }
    root->setFixedSize(size);
    if(root->layout())root->layout()->activate();
    for(auto* widget:widgets)widget->update();
}

