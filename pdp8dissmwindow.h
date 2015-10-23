#ifndef PDP8DISSMWINDOW_H
#define PDP8DISSMWINDOW_H


#include <QWidget>
#include <QPlainTextEdit>

#include "pdp8i.h"

class PDP8DissmArea
{
public:
    PDP8DissmArea(PDP8DissmWindow *dissmWindow) : QWidget(dissmWindow) {
        dissmWindow = editor;
    }

    QSize sizeHint() const Q_DECL_OVERRIDE {
        return QSize(dissmWindow->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE {
        dissmWindow->lineNumberAreaPaintEvent(event);
    }

private:
    PDP8DissmWindow *dissmWindow;
};

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



#endif // PDP8DISSMWINDOW_H
