
#include "qtty.h"
#include <QScrollBar>
#include <QException>
#include <QtCore/QDebug>
#include <QDebug>
#include <QFontMetrics>

struct ASR33Keys {
    QMap<int,int> keys;
    int operator[](int i) {
        auto itr = keys.constFind(i);
        if(itr != keys.cend()) return itr.value();
        return -1;
    }
    ASR33Keys() {
        keys[Qt::Key_A] = 0301;
        keys[Qt::Key_B] = 0302;
        keys[Qt::Key_C] = 0303;
        keys[Qt::Key_D] = 0304;
        keys[Qt::Key_E] = 0305;
        keys[Qt::Key_F] = 0306;
        keys[Qt::Key_G] = 0307;
        keys[Qt::Key_H] = 0310;
        keys[Qt::Key_I] = 0311;
        keys[Qt::Key_J] = 0312;
        keys[Qt::Key_K] = 0313;
        keys[Qt::Key_L] = 0314;
        keys[Qt::Key_M] = 0315;
        keys[Qt::Key_N] = 0316;
        keys[Qt::Key_O] = 0317;
        keys[Qt::Key_P] = 0320;
        keys[Qt::Key_Q] = 0321;
        keys[Qt::Key_R] = 0322;
        keys[Qt::Key_S] = 0323;
        keys[Qt::Key_T] = 0324;
        keys[Qt::Key_U] = 0325;
        keys[Qt::Key_V] = 0326;
        keys[Qt::Key_W] = 0327;
        keys[Qt::Key_X] = 0330;
        keys[Qt::Key_Y] = 0331;
        keys[Qt::Key_Z] = 0332;

        keys[Qt::Key_0] = 0260;
        keys[Qt::Key_1] = 0261;
        keys[Qt::Key_2] = 0262;
        keys[Qt::Key_3] = 0263;
        keys[Qt::Key_4] = 0264;
        keys[Qt::Key_5] = 0265;
        keys[Qt::Key_6] = 0266;
        keys[Qt::Key_7] = 0267;
        keys[Qt::Key_8] = 0270;
        keys[Qt::Key_9] = 0271;

        keys[Qt::Key_Return] = 0215;
        keys[Qt::Key_Space] = 0240;
        keys[Qt::Key_Alt] = 0375;
        keys[Qt::Key_Backspace] = 0377;
    }
};

static ASR33Keys keys;



static void InternalProcessCharater(int c, QTextCursor& tc) {
        switch(c) {
        case '\n':
            if(!tc.atBlockEnd()) tc.movePosition(QTextCursor::End);
            tc.insertText(QString('\n'));
            break;
        case '\r':
            tc.movePosition(QTextCursor::StartOfLine);
            break;
          case '\b':
            if(!tc.atBlockStart()){
                tc.movePosition(QTextCursor::Left);
            }
            break;
        case '\t':
            if(!tc.atEnd()) {
                QTextCursor backup = tc;
                int backup_pos = tc.position(); // position in line
                int npos = (tc.columnNumber() & 7) + 8; // start of tab
                backup.movePosition(QTextCursor::End);
                int epos = backup.columnNumber();
                if(npos > epos) {
                    // need to add spaces
                    QString s(' ',npos - epos);

                }
                // ugh HELP ME
            } else
                tc.insertText(QString('\n'));
            break;
        case 0:
            break; // do nothing on nulls
        default:
            if(!tc.atEnd()) tc.deleteChar(); // should type over existing charater
            tc.insertText(QString(QChar::fromLatin1(c)));
        }
}

QTTY::QTTY(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false),overline(false)
{
    connect(this,&QTTY::putDataSignalC,this,&QTTY::putDataSlotC);
    connect(this,&QTTY::putDataSignalS,this,&QTTY::putDataSlotS);
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);
    int id = QFontDatabase::addApplicationFont(":/fonts/veteran typewriter.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    // set font and tab
    QFont monospace(family);
    monospace.setStyleHint(QFont::Monospace);
    monospace.setFixedPitch(true);
    monospace.setPointSize(10);

    const int tabStop = 8;  // 8 characters
    QFontMetrics metrics(monospace);
    setTabStopWidth(tabStop * metrics.width(' '));
    setFont(monospace);


    setReadOnly(true);
}


 void QTTY::putData(int c) {
     emit putDataSignalC(c); // Have to do this cross threads
 }
 void QTTY::putData(const QString& s) {
     emit putDataSignalS(s); // Have to do this cross threads
 }
 void QTTY::putDataSlotS(const QString& s) {
     QTextCursor tc = textCursor();
     tc.beginEditBlock();

     for(QChar c : s) InternalProcessCharater(c.toLatin1(),tc);
     tc.endEditBlock();
     setTextCursor(tc); // we are doing SOMETHING so run it at the end

     //insertPlainText(QString(c));
     QScrollBar *bar = verticalScrollBar();
     bar->setValue(bar->maximum());
 }

 void QTTY::putDataSlotC(int c) {
     QTextCursor tc = textCursor();
     InternalProcessCharater(c,tc);
     setTextCursor(tc); // we are doing SOMETHING so run it at the end

     //insertPlainText(QString(c));
     QScrollBar *bar = verticalScrollBar();
     bar->setValue(bar->maximum());
 }
void QTTY::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void QTTY::keyPressEvent(QKeyEvent *e)
{
    int k = keys[e->key()];
    if(k!= -1) {
        emit keyboardSend(k);
        return;
    }
    QString text = e->text(); // too expensive to keep making a string for EACH char
    if(text.count() >0) {
        Q_ASSERT(text.count() == 1);
        QChar c = text[0];
        // just in c ase we miss one
        if(c.isPrint()) {
            qDebug() << "Missing Printable key? (" << text << ")[\\" << QString::number(text[0].toLatin1(),8) << "]";
            emit keyboardSend(text[0].toLatin1());
        } else {
            qDebug() << "Invalid Non-Printable key? [\\" << QString::number(text[0].toLatin1(),8) << "]";
        }
    }
}

void QTTY::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void QTTY::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void QTTY::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
