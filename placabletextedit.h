#ifndef STUPIDVT52WIDGET_H
#define STUPIDVT52WIDGET_H

#include <QPlainTextEdit>
#include <QTimer>
#include "tty_vt52.h"
#include "terminal.h"

class PlacableTextEdit : public QWidget
{
    Q_OBJECT
public:
    explicit PlacableTextEdit(QWidget *parent = 0);

public:
    void putString(const QString& s);
public slots:
    // internal slots that are used so we can be thread safe with other stuff
    void putChar(int c);
    void blinker();
private slots:
    void putCharSlot(int c);
signals:
    void putCharSignal(int c);
    void keyboardSend(int k);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);
    void paintEvent(QPaintEvent *e);
private:
    bool localEchoEnabled;
    bool wordWrapEnabled;
    bool overline;
    QTimer _blinker;
    size_t _blinkCounter;
    QRect _consoleRect;
    QPoint _cursor;
    QSize _fontSize;
    QFont _font;
    std::unique_ptr<Terminal::BasicTerminal> _term;
};
#endif // STUPIDVT52WIDGET_H
