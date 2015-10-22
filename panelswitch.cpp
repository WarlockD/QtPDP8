#include "panelswitch.h"
#include "QPainter.h"
#include <QMouseEvent>

static QColor dec_brown(0x93,0x4f,0x1f);
static QColor dec_orange(0xb6,0x67,0x02);

PanelSwitch::PanelSwitch(QWidget *parent) : QWidget(parent), m_tag(0),m_startState(false),m_down(false),m_momantary(false),m_lightShade(dec_orange),m_darkShade(dec_orange.dark())
{
  setMouseTracking(true);
  setBaseSize(sizeHint());
}

QSize PanelSwitch::sizeHint() const
{
    return QSize(40,80);
}
/*! \reimp */
bool PanelSwitch::event(QEvent *e)
{
    return QWidget::event(e);
}
/*!
    \reimp
*/

QSize PanelSwitch::minimumSizeHint() const
{
    return sizeHint();
}
void PanelSwitch::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit switchPressed(m_down=!m_down,m_tag);
        update();
    } else QWidget::mousePressEvent(event);
}
void PanelSwitch::setDown(bool down) {
    if(m_down != down) {
        m_down = down;
        update(); // update the look
    }
}

#define POLYPART(...) QPolygon({ __VA_ARGS__ })

static const QPolygon switch_on_dark_poly[] = {
    POLYPART(QPoint(0,40),QPoint(40,40),QPoint(30,0),QPoint(10,0),QPoint(0,40)),
    POLYPART(QPoint(0,40),QPoint(40,40),QPoint(30,0),QPoint(10,0),QPoint(0,40)),
    POLYPART(QPoint(0,40),QPoint(0,80),QPoint(10,70),QPoint(0,40)),
    POLYPART(QPoint(40,40),QPoint(40,80),QPoint(30,70),QPoint(40,40)),
    POLYPART(QPoint(0,80), QPoint(40,80), QPoint(30,70),QPoint(10,70),QPoint(0,80))
};
static const QPolygon switch_on_light_poly[] = {
    POLYPART(QPoint(0,40),QPoint(10,70),QPoint(30,70),QPoint(40,40))
};

static const QPolygon switch_off_dark_poly[] = {
    POLYPART(QPoint(0,40),QPoint(40,40),QPoint(30,80),QPoint(10,80),QPoint(0,40)),
    POLYPART(QPoint(0,40),QPoint(40,40),QPoint(30,80),QPoint(10,80),QPoint(0,40)),
    POLYPART(QPoint(0,40),QPoint(0,0),QPoint(10,10),QPoint(0,40)),
    POLYPART(QPoint(40,40),QPoint(40,0),QPoint(30,10),QPoint(40,40)),
    POLYPART(QPoint(0,0), QPoint(40,0), QPoint(30,10),QPoint(10,10),QPoint(0,0))
};
static const QPolygon switch_off_light_poly[] = {
    POLYPART(QPoint(0,40),QPoint(10,10),QPoint(30,10),QPoint(40,40))
};

void PanelSwitch::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
     painter.scale(width() / 40.0, height() / 80.0);
    if(m_down) {
        painter.setPen(Qt::white);
        painter.setBrush(m_lightShade);
        for(QPolygon a : switch_off_light_poly) painter.drawPolygon(a);
        painter.setBrush(m_darkShade);
       for(QPolygon a : switch_off_dark_poly) painter.drawPolygon(a);
    } else {
        painter.setPen(Qt::white);
        painter.setBrush(m_lightShade);
        for(QPolygon a : switch_on_light_poly) painter.drawPolygon(a);
        painter.setBrush(m_darkShade);
       for(QPolygon a : switch_on_dark_poly) painter.drawPolygon(a);
    }
}
