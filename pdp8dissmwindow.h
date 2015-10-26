#ifndef PDP8DISSMWINDOW_H
#define PDP8DISSMWINDOW_H


#include <QWidget>
#include <QPlainTextEdit>

#include "pdp8i.h"



class PDP8DissmWindow : public QPlainTextEdit
{
      Q_OBJECT
public:
    PDP8DissmWindow(QWidget *parent = 0);
    void dissamAreaPaintEvent(QPaintEvent *event);
    int dissamAreaWidth();
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private slots:
    void updateDissamAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateDissamArea(const QRect &, int);

private:
    QWidget *dissamArea;
};

class PDP8DissmArea : public QWidget
{
public:
    PDP8DissmArea(PDP8DissmWindow *window) : QWidget(dissmWindow) , dissmWindow(window){}

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(dissmWindow->dissamAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        dissmWindow->dissamAreaPaintEvent(event);
    }

private:
    PDP8DissmWindow *dissmWindow;
};

#endif // PDP8DISSMWINDOW_H
