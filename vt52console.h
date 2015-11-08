#ifndef VT52CONSOLE_H
#define VT52CONSOLE_H

#include <QPlainTextEdit>
#include <QQueue>
#include <QVector>
#include <QTimer>
#include "tty_vt52.h"

class VT52Console : public QPlainTextEdit, public PDP8::SerialInterface
{
    Q_OBJECT
    QQueue<unsigned char> _dataOut;
    QTimer* _cursorBlink;
    PDP8::VT52_Helper helper;


public:
    void refreshScreen();
    explicit VT52Console(QWidget *parent = 0);
    bool haveData() const;
    int received();

    // Ok, since we are moving byte data accross threads, to make sure QWidget dosn't
    // freekout, we need to make sure it goes into the singal/slot section.  This means
    // trasmit emits to trasmitSignal, witch in turn gotes to trasmitSlot
    void trasmit(unsigned char data);
signals:
    void trasmitSignal(unsigned char data);
public slots:
     void trasmitSlot(unsigned char data);
     void onTimer();
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);



};


#endif // VT52CONSOLE_H
