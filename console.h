#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>

class Console : public QPlainTextEdit
{
    Q_OBJECT

signals:
    void getData(const QChar data);
    void putDataSignal(const QChar data);
public:
    explicit Console(QWidget *parent = 0);

    void putData(const QByteArray &data);
    void putData(const QChar c);
    void putData(const QString& s);
    void setLocalEchoEnabled(bool set);

public slots:
    void putDataSlot(const QChar c);
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void contextMenuEvent(QContextMenuEvent *e);

private:
    bool localEchoEnabled;
    bool overline;
};

#endif // CONSOLE_H
