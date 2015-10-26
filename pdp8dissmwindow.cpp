#include "pdp8dissmwindow.h"
#include <QPainter>
#include <QTextBlock>
#include <QPlainTextEdit>
PDP8DissmWindow::PDP8DissmWindow(QWidget *parent) : QPlainTextEdit(parent)
{
    dissamArea = new PDP8DissmArea(this);

    connect(this, &PDP8DissmWindow::blockCountChanged, this, &PDP8DissmWindow::updateDissamAreaWidth);
    connect(this, &PDP8DissmWindow::updateRequest, this, &PDP8DissmWindow::updateDissamArea);
    connect(this, &PDP8DissmWindow::cursorPositionChanged, this, &PDP8DissmWindow::highlightCurrentLine);
    updateDissamAreaWidth(0);
    highlightCurrentLine();
}

    void PDP8DissmWindow::updateDissamAreaWidth(int newBlockCount)
    {
        setViewportMargins(dissamAreaWidth(), 0, 0, 0);
    }
    void PDP8DissmWindow::highlightCurrentLine()
    {
        QList<QTextEdit::ExtraSelection> extraSelections;

        if (!isReadOnly()) {
            QTextEdit::ExtraSelection selection;

            QColor lineColor = QColor(Qt::yellow).lighter(160);

            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            extraSelections.append(selection);
        }

        setExtraSelections(extraSelections);
    }
    int PDP8DissmWindow::dissamAreaWidth()
    {
        int digits = 1;
        int max = qMax(1, blockCount());
        while (max >= 10) {
            max /= 10;
            ++digits;
        }

        int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

        return space;
    }
    void PDP8DissmWindow::resizeEvent(QResizeEvent *event)
    {
        QPlainTextEdit::resizeEvent(event);

        QRect cr = contentsRect();
        dissamArea->setGeometry(QRect(cr.left(), cr.top(), dissamAreaWidth(), cr.height()));
    }
    void PDP8DissmWindow::updateDissamArea(const QRect &rect, int dy)
    {
        if (dy)
            dissamArea->scroll(0, dy);
        else
            dissamArea->update(0, rect.y(), dissamArea->width(), rect.height());

        if (rect.contains(viewport()->rect()))
            updateDissamAreaWidth(0);
    }
void PDP8DissmWindow::dissamAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(dissamArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, dissamArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}
