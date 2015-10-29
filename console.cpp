#include "console.h"


#include <QScrollBar>
#include <QException>
#include <QtCore/QDebug>
#include <QDebug>

Console::Console(QWidget *parent)
    : QPlainTextEdit(parent)
    , localEchoEnabled(false)
{
    document()->setMaximumBlockCount(100);
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    setPalette(p);

}

void Console::putData(const QByteArray &data)
{
    insertPlainText(QString(data));

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}
void Console::putData(const QString& s) {
    insertPlainText(s);

    QScrollBar *bar = verticalScrollBar();
    bar->setValue(bar->maximum());
}
 void Console::putData(const QChar c) {
     insertPlainText(QString(c));
     QScrollBar *bar = verticalScrollBar();
     bar->setValue(bar->maximum());
 }

void Console::setLocalEchoEnabled(bool set)
{
    localEchoEnabled = set;
}

void Console::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    default:
        if (localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);

    }
    QString text = e->text(); // too expensive to keep making a string for EACH char
    Q_ASSERT(text.count() == 1);


    emit getData(text[0].toLatin1());
}

void Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}
