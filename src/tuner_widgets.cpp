#include <utility>
#include "tuner_widgets.hpp"
#include <QPainter>
#include <QPainterPath>
#include <algorithm>
#include <cmath>

namespace {
constexpr double pi = 3.14159265358979323846;
QColor ink(const QWidget* w) { return w->palette().color(QPalette::WindowText); }
QColor accent(const QWidget* w) { return w->palette().color(QPalette::Highlight); }
void text(QPainter& p, QRectF r, const QString& s, QColor color, int size, bool bold = false) {
    auto f = p.font(); f.setPixelSize(size); f.setBold(bold); p.setFont(f);
    p.setPen(color); p.drawText(r, Qt::AlignCenter, s);
}
void panel(QPainter& p, QWidget* w) {
    p.setPen(QPen(w->palette().color(QPalette::Mid), 1.5));
    p.setBrush(w->palette().color(QPalette::Base));
    p.drawRoundedRect(w->rect().adjusted(2, 2, -2, -2), 16, 16);
}
QPointF point(QPointF o, double r, double c) {
    const auto a = c / 50 * pi / 2;
    return {o.x() + r * std::sin(a), o.y() - r * std::cos(a)};
}
}
AnalogGauge::AnalogGauge(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(370); setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAccessibleName("Analóg hangoló, mínusz ötven és plusz ötven cent között");
}
void AnalogGauge::setCents(double cents, bool valid) {
    cents_ = std::clamp(cents, -50.0, 50.0); valid_ = valid; update();
}
void AnalogGauge::setReadout(const QString& note, const QString& target, const QString& status) {
    note_ = note; target_ = target; status_ = status;
    setAccessibleDescription(note + ", " + target + ", " + status); update();
}
void AnalogGauge::paintEvent(QPaintEvent*) {
    QPainter p(this); p.setRenderHint(QPainter::Antialiasing);
    // Use a logical canvas so the complete instrument scales together.
    const double scale = std::min(width()/800.0, height()/440.0);
    p.translate((width()-800*scale)/2, (height()-440*scale)/2); p.scale(scale, scale);
    const QPointF origin(400, 270); const double radius = 225;
    QPainterPath arc;
    for (int c=-50;c<=50;++c) { auto pt=point(origin,radius,c); if(c==-50) arc.moveTo(pt); else arc.lineTo(pt); }
    p.setPen(QPen(palette().color(QPalette::Mid),3)); p.drawPath(arc);
    QPainterPath zone;
    for(int c=-3;c<=3;++c) {auto pt=point(origin,radius-9,c); if(c==-3) zone.moveTo(pt); else zone.lineTo(pt);}
    p.setPen(QPen(accent(this),16)); p.drawPath(zone);
    for(int c=-50;c<=50;c+=5) {
        bool major=c%25==0;
        p.setPen(QPen(ink(this),major?4:2,Qt::SolidLine,Qt::RoundCap));
        p.drawLine(point(origin,radius,c),point(origin,radius-(major?22:12),c));
        if(major) {auto pt=point(origin,radius+29,c);text(p,{pt.x()-38,pt.y()-20,76,40},c>0?QString("+%1").arg(c):QString::number(c),ink(this),27,true);}
    }
    if(valid_) {
        p.setPen(QPen(accent(this),6,Qt::SolidLine,Qt::RoundCap));
        p.drawLine(origin,point(origin,radius-18,cents_));
        p.setPen(Qt::NoPen);p.setBrush(accent(this));p.drawEllipse(origin,9,9);
    }
    text(p,{240,285,320,60},note_,ink(this),58,true);
    text(p,{220,347,360,28},target_,ink(this),23,true);
    text(p,{150,388,500,36},status_,ink(this),25,true);
    text(p,{35,300,160,35},"Túl mély",ink(this),21);
    text(p,{605,300,160,35},"Túl magas",ink(this),21);
}
PitchScale::PitchScale(QWidget* parent):QWidget(parent) {setMinimumHeight(145);setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);setAccessibleName("Húrok és mért hangmagasság");}
void PitchScale::setPreset(const strings::TuningPreset* p,double c){preset_=p;calibration_=c;update();}
void PitchScale::setFrequency(double f){frequencyHz_=f;update();}
void PitchScale::paintEvent(QPaintEvent*) {
    QPainter p(this);p.setRenderHint(QPainter::Antialiasing);panel(p,this);
    text(p,{18,8,90,26},"Húrok",ink(this),22,true);
    if(!preset_||preset_->targets.empty()) return;
    const double low=preset_->targets.front().hz*calibration_/440;
    const double high=preset_->targets.back().hz*calibration_/440;
    // Extra octaves keep out-of-register measurements visible.
    const double min=low/2,max=high*2,left=35,right=width()-35,y=83;
    auto x=[&](double hz){return left+std::log2(hz/min)/std::log2(max/min)*(right-left);};
    p.setPen(QPen(ink(this),3,Qt::SolidLine,Qt::RoundCap));p.drawLine(QPointF(left,y),QPointF(right,y));
    for(const auto& t:preset_->targets){
        double tx=x(t.hz*calibration_/440);
        const bool selected=frequencyHz_>0&&std::abs(strings::centsBetween(frequencyHz_,t.hz*calibration_/440))<=50;
        if(selected){QColor fill=accent(this);fill.setAlpha(40);p.setPen(Qt::NoPen);p.setBrush(fill);p.drawRoundedRect(QRectF(tx-27,37,54,98),12,12);}
        p.setPen(QPen(ink(this),3));p.drawLine(QPointF(tx,y-10),QPointF(tx,y+10));
        text(p,{tx-25,40,50,32},QString::number(t.number),ink(this),27,true);
        text(p,{tx-32,99,64,32},QString::fromStdString(t.note),ink(this),24,true);
    }
    if(frequencyHz_>0){double px=x(std::clamp(frequencyHz_,min,max));p.setBrush(accent(this));p.setPen(QPen(palette().color(QPalette::Base),2));p.drawEllipse(QPointF(px,y),8,8);}
}
InputLevelMeter::InputLevelMeter(QWidget* parent):QWidget(parent){setMinimumHeight(115);setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);setAccessibleName("Bemeneti jelszint");}
void InputLevelMeter::setLevel(double r,double p,bool c){rmsDb_=r;peakDb_=p;clipped_=c;update();}
void InputLevelMeter::setLimits(double m,double w){minDb_=m;warningDb_=w;update();}
void InputLevelMeter::paintEvent(QPaintEvent*) {
    QPainter p(this);p.setRenderHint(QPainter::Antialiasing);panel(p,this);
    const bool low=rmsDb_<minDb_,high=clipped_||peakDb_>=warningDb_;
    const QString state=high?"Túl erős jel":low?"Pengess meg egy húrt":"Megfelelő jelszint";
    text(p,{18,10,145,30},"Hangerő",ink(this),24,true);
    text(p,{width()/2.0-160,10,320,30},state,ink(this),21,true);
    const double left=38,right=width()-38,y=79;
    auto x=[&](double db){return left+std::clamp((db+70)/70,0.0,1.0)*(right-left);};
    QColor safe=accent(this);safe.setAlpha(35);p.setPen(Qt::NoPen);p.setBrush(safe);
    p.drawRoundedRect(QRectF(x(minDb_),y-9,x(warningDb_)-x(minDb_),30),7,7);
    for(int i=0;i<40;++i){double db=-70+(i+1)*70.0/40;QColor color=palette().color(QPalette::Mid);
        if(db<=rmsDb_)color=(high?QColor("#c14b3e"):accent(this));
        p.setBrush(color);p.drawRoundedRect(QRectF(left+i*(right-left)/40,y,(right-left)/40-4,13),3,3);}
    for(auto pair:{std::pair<double,QString>{minDb_,"MIN"},{warningDb_,"MAX"}}){
        p.setPen(QPen(ink(this),2));p.drawLine(QPointF(x(pair.first),y-14),QPointF(x(pair.first),y-3));
        text(p,{x(pair.first)-25,y-40,50,24},pair.second,ink(this),17,true);}
}
