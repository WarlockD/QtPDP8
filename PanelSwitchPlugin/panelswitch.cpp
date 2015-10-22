#include "panelswitch.h"
#include "QPainter.h"


PanelSwitch::PanelSwitch(QWidget *parent) : QCheckBox(parent), _on(false), _startState(false), _toggle(false), _lightShade(0xFF, 0x93, 0x4F, 0x1F), _darkShade(_lightShade.dark())
{

}


QSize PanelSwitch::sizeHint() const
{
  //  Q_D(const QCheckBox);
    return QSize(40,80);
    /*

    if (d->sizeHint.isValid())
        return d->sizeHint;
    ensurePolished();
    QFontMetrics fm = fontMetrics();
    QStyleOptionButton opt;
    initStyleOption(&opt);
    QSize sz = style()->itemTextRect(fm, QRect(), Qt::TextShowMnemonic, false,
                                     text()).size();
    if (!opt.icon.isNull())
        sz = QSize(sz.width() + opt.iconSize.width() + 4, qMax(sz.height(), opt.iconSize.height()));
    d->sizeHint = (style()->sizeFromContents(QStyle::CT_CheckBox, &opt, sz, this)
                  .expandedTo(QApplication::globalStrut()));
    return d->sizeHint;
    */
}


/*!
    \reimp
*/

QSize PanelSwitch::minimumSizeHint() const
{
    return sizeHint();
}

void PanelSwitch::paintEvent(QPaintEvent *)
{
    static const QPointF polygon_switch_on[] = {
                        QPointF(0.0f, 50.0f), QPointF(100.0f, 50.0f), QPointF(80.0f, 0.0f),QPointF(20.0f, 0),
                        QPointF(20.0f, 90.0f), QPointF(80.0f, 90.0f), QPointF(100.0f, 50.0f), QPointF(0.0f, 50.0f),
                        QPointF(0.0f, 100.0f), QPointF(100.0f, 100.0f), QPointF(100.0f, 50.0f), QPointF(80.0f, 90.0f), QPointF(20.0f, 90.0f), QPointF(0.0f,50.0f),
                       };
    static const QPointF polygon_switch_off[] = {
        QPointF(0.0f, 50.0f), QPointF(100.0f, 50.0f), QPointF(80.0f, 0.0f), QPointF(20.0f, 0) ,
                        QPointF(20.0f, 100.0f), QPointF(80.0f, 100.0f), QPointF(100.0f, 50.0f), QPointF(0.0f, 50.0f) ,
                        QPointF(0.0f, 0.0f), QPointF(0.0f, 100.0f), QPointF(0.0f, 50.0f), QPointF(20.0f, 90.0f), QPointF(20.0f, 90.0f), QPointF(0.0f,50.0f),
                  //      QPointF(0.2f, 1.0f), QPointF(0.8f, 1.0f), QPointF(1.0f, 0.5f), QPointF(0.0f, 0.5f),
                    };
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
        painter.scale(width() / 100.0, height() / 100.0);
    if(isChecked()) {
        painter.setPen(Qt::white);
        painter.setBrush(_lightShade);

        painter.drawPolygon(polygon_switch_on+4, 4);

         painter.setBrush(_darkShade);
        painter.drawPolygon(polygon_switch_on, 4);
        painter.drawPolygon(polygon_switch_on+8, 6);

    } else {
        painter.setPen(Qt::white);
        painter.setBrush(_lightShade);
        painter.drawPolygon(polygon_switch_off, 4);

        painter.drawPolygon(polygon_switch_off+8, 6);
        painter.setBrush(_darkShade);

         painter.drawPolygon(polygon_switch_off+4, 4);
    }
  //  QPointF fl;




   // QColor hourColor(127, 0, 127);
//    QColor minuteColor(0, 127, 127, 191);
/*

   // QTime time = QTime::currentTime();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
   // painter.translate(width() / 2, height() / 2);
    painter.scale(width() / 100.0, height() / 100.0);
    if(_on) {
        painter.setPen(Qt::white);
        painter.setBrush(_lightShade);

        painter.drawPolygon(polygon_switch_on+4, 4);

         painter.setBrush(_darkShade);
        painter.drawPolygon(polygon_switch_on, 4);
        painter.drawPolygon(polygon_switch_on+8, 6);
    } else {
        painter.setPen(Qt::white);
        painter.setBrush(_lightShade);
        painter.drawPolygon(polygon_switch_off, 4);

        painter.drawPolygon(polygon_switch_off+8, 6);
        painter.setBrush(_darkShade);

         painter.drawPolygon(polygon_switch_off+4, 4);
    }
*/

}
