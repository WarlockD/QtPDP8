#ifndef QTTY2_H
#define QTTY2_H

#include <QWidget>
#include <QTimer>
#include "tty_vt52.h"
#include "terminal.h"
class QTTY2 : public QWidget
{
    Q_OBJECT
    Terminal::BasicTerminal* _terminal;
    QFont _font;
    QTimer _blinker;
public:
    explicit QTTY2(QWidget *parent = 0);
    ~QTTY2();
public:
    void putData(int c);
    void putData(const QString& s);
    void setLocalEchoEnabled(bool set);

private slots:
    // internal slots that are used so we can be thread safe with other stuff
    void putDataSlotC(int c);
    void putDataSlotS(const QString& c);
  //  void blinker();
signals:
    void putDataSignalC(int c);
    void putDataSignalS(const QString& s);
    void keyboardSend(int k);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void paintEvent(QPaintEvent *event);

};

#endif // QTTY2_H
