#include "placabletextedit.h"
#include <QPainter>
#include <QKeyEvent>
#include <QFontDatabase>
#include <QScrollBar>
#include <QDebug>
#include <QTextBlock>
#include "termina_vt52.h"
//QLatin1Char('\n')
//QLatin1String("<br>\n")
struct blank_screen{
     QString clearScreen;
     QString clearLine;
     blank_screen() {
         clearLine = QString(80, QLatin1Char(' '));
         clearLine.append(QLatin1String("\n"));
         for(int i=0;i<24;i++) clearScreen += clearLine;

     }
};
blank_screen blankscreen;


PlacableTextEdit::PlacableTextEdit(QWidget *parent)
    : QWidget(parent)
    , localEchoEnabled(false),overline(false), _cursor(0,0), wordWrapEnabled(true)
{
    connect(this,&PlacableTextEdit::putCharSignal,this,&PlacableTextEdit::putCharSlot);

    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
    int id = QFontDatabase::addApplicationFont(":/fonts/veteran typewriter.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    // set font and tab
    QFont monospace("Courier New");
    monospace.setStyleHint(QFont::Monospace);
    monospace.setFixedPitch(true);
    monospace.setPointSize(12);
    QFontMetrics metrics(monospace);
  //  setTabStopWidth(tabStop * metrics.width(' '));
    setFont(monospace);

    _font = monospace;
    _fontSize  = QSize(metrics.width(' '), metrics.height());
    connect(&_blinker,&QTimer::timeout,this,&PlacableTextEdit::blinker);
    _blinker.start(250);
    _term = std::make_unique<Terminal::VT52>();
}


void PlacableTextEdit::blinker() {
    update();
    _blinkCounter++;
}


void PlacableTextEdit::putChar(int c){ emit putCharSignal(c); }

void PlacableTextEdit::putCharSlot(int c){
    if(_term) {

       const char* reply= _term->putChar(c);

       if(reply) {
           while(*reply) emit keyboardSend(*reply++);
       }
       update();
    }
}

void PlacableTextEdit::keyPressEvent(QKeyEvent *e)
{
    (void)e;
    /*
    // testing
    switch(e->key()) {
        case Qt::Key_Left: cursorLeft(); break;
        case Qt::Key_Right: cursorRight(); break;
        case Qt::Key_Up: cursorUp(); break;
        case Qt::Key_Down: cursorDown(); break;
    case Qt::Key_Return: putChar('\r'); putChar('\n');break;
    default:
        QString text = e->text(); // too expensive to keep making a string for EACH char
        if(text.count() >0) {
            Q_ASSERT(text.count() == 1);
            QChar c = text[0];
            putChar(c);
            // just in c ase we miss one
            if(c.isPrint()) {
                qDebug() << "Missing Printable key? (" << text << ")[\\" << QString::number(text[0].toLatin1(),8) << "]";
                emit keyboardSend(text[0].toLatin1());
            } else {
                qDebug() << "Invalid Non-Printable key? [\\" << QString::number(text[0].toLatin1(),8) << "]";
            }
        }
    }
    */
}
void PlacableTextEdit::paintEvent(QPaintEvent *e)
{
    (void)e;
    if(_term) {
        QPainter p(this);
        QFontMetrics metrics(p.font());
        QRect r(0,0,metrics.width(QLatin1Char(' '))*80,metrics.height());
       p.fillRect(rect(),Qt::black);
          p.setPen(Qt::green);
       p.setFont(p.font());
        for(int i=0;i<24;i++) {
                QString s = QString::fromStdString(_term->getLine(i));
                QRect r (0, metrics.height()*i,metrics.width(s),metrics.height());
                p.drawText(r,s);
        }
    }

}


void PlacableTextEdit::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void PlacableTextEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void PlacableTextEdit::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
