#include "stupidvt52widget.h"
#include <QPainter>

StupidVT52Widget::StupidVT52Widget()
{

}

 void StupidVT52Widget::paintEvent( QPaintEvent* pe ){
       QPainter paint(this);
       drawContentsStupid(paint);

 }

 void StupidVT52Widget::drawBackground(QPainter& painter, const QRect& rect, const QColor& color,bool useOpacitySetting){
     painter.fillRect(contentsRect, backgroundColor);
     /*
     QRect scrollBarArea = _scrollBar->isVisible() ?
                                        rect.intersected(_scrollBar->geometry()) :
                                        QRect();
            QRegion contentsRegion = QRegion(rect).subtracted(scrollBarArea);
            QRect contentsRect = contentsRegion.boundingRect();

            if ( HAVE_TRANSPARENCY && qAlpha(_blendColor) < 0xff && useOpacitySetting )
            {
                QColor color(backgroundColor);
                color.setAlpha(qAlpha(_blendColor));

                painter.save();
                painter.setCompositionMode(QPainter::CompositionMode_Source);
                painter.fillRect(contentsRect, color);
                painter.restore();
            }
            else
                painter.fillRect(contentsRect, backgroundColor);

          //  painter.fillRect(scrollBarArea,_scrollBar->palette().background());
          */
 }
void StupidVT52Widget::drawContentsStupid(QPainter& painter) {
    QString line;
    line.reserve(_usedColumns);
    for(int y = 0; y< _usedLines; y++) {
        for (int x=0; x < _usedColumns; x++) line[x] = _screen[y][x].c;
        QRect textArea = QRect( _leftMargin+_fontWidth*x , _topMargin+_fontHeight*y , _fontWidth*_usedColumns , _fontHeight);
        painter.fillRect(textArea,Qt::black);
        painter.setPen(Qt::white);
        painter.drawText(textArea,0,line); // so very simple
    }

}
 void StupidVT52Widget::drawContents(QPainter& painter, const QRect& rect){
     // this is a simple test of redraw EVEYTHING
    }
