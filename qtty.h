#ifndef QTTY_H
#define QTTY_H

#include <QPlainTextEdit>

class QTTY : public QPlainTextEdit
{
    Q_OBJECT


public:
    explicit QTTY(QWidget *parent = 0);

public:
    void putData(int c);
    void putData(const QString& s);
    void setLocalEchoEnabled(bool set);

private slots:
    // internal slots that are used so we can be thread safe with other stuff
    void putDataSlotC(int c);
    void putDataSlotS(const QString& c);
signals:
    void putDataSignalC(int c);
    void putDataSignalS(const QString& s);
    void keyboardSend(int k);

protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:
    bool localEchoEnabled;
    bool overline;
};

#endif // QTTY_H
