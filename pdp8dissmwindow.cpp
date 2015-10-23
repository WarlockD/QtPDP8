#include "pdp8dissmwindow.h"

PDP8DissmWindow::PDP8DissmWindow(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);
    dissamArea = new PDP8DissmArea(this);

    connect(this, &PDP8DissmWindow::blockCountChanged, this, &PDP8DissmWindow::updateDissamAreaWidth);
    connect(this, &PDP8DissmWindow::updateRequest, this, &PDP8DissmWindow::updateDissamArea);
    connect(this, &PDP8DissmWindow::cursorPositionChanged, this, &PDP8DissmWindow::highlightCurrentLine);

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

void PDP8DissmWindow::dissamAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
