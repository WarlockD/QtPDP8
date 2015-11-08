#include "vt52console.h"

VT52Console::VT52Console(QWidget *parent) : QPlainTextEdit(parent)
{
    connect(this,&VT52Console::trasmitSignal,this,&VT52Console::trasmitSlot);
    QFont consoleFont;
    consoleFont.setFamily("Courier New");
    setFont(consoleFont);
    setReadOnly(true);
    setWordWrapMode(QTextOption::NoWrap);
    refreshScreen();
}
void VT52Console::trasmit(unsigned char data) {
    emit trasmitSignal(data);
}

bool VT52Console::haveData() const {
    return !_dataOut.empty();
}
int VT52Console::received() {
    if(_dataOut.empty()) return -1;
    return _dataOut.dequeue();
}
void VT52Console::refreshScreen() {
    setPlainText(QString::fromStdString(helper.getScreen()));
}



void VT52Console::trasmitSlot(unsigned char data){
    if(helper.processChar(data)){
        _dataOut.enqueue(033);
        _dataOut.enqueue('K');
    }
}

void VT52Console::onTimer(){

}

void VT52Console::keyPressEvent(QKeyEvent *e)
{
    // simple and sweet.
    QString text = e->text(); // too expensive to keep making a string for EACH char
    if(text.count() >0) {
        Q_ASSERT(text.count() == 1);
        QChar c = text[0];
         _dataOut.enqueue(c.toLatin1());

       // if(c.isPrint()) { emit getData(text[0].toLatin1()); }
    }
    /*
    switch (e->key()) {
    case Qt::Key_Backspace:
    case Qt::Key_Left:
    case Qt::Key_Right:
    case Qt::Key_Up:
    case Qt::Key_Down:
        break;
    case Qt::Key_Return:
        emit getData(0215);
        break;
    default:
        if (localEchoEnabled)
            QPlainTextEdit::keyPressEvent(e);


        break;
    }
   */
}

void VT52Console::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    setFocus();
}

void VT52Console::mouseDoubleClickEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
}

void VT52Console::contextMenuEvent(QContextMenuEvent *e)
{
    Q_UNUSED(e)
}

