/*
CoreBox is combination of some common desktop apps

CoreBox is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, see {http://www.gnu.org/licenses/}. */

#include "coreedit.h"


coreedit::coreedit(QString fPath, QWidget *parent) : QPlainTextEdit(parent)
{
    // It will need in future
    filePath = fPath;

    cHighlighter *highlighter = new cHighlighter(document());
    highlighter->setObjectName("cHighlighter");
    QString bcolor(Utilities::getStylesheetValue()->value("@color01").toString());
    setStyleSheet("QWidget{background-color: " + bcolor + "; border: none;} QMenu{border: 1px solid gray;}");

    lineNumberArea = new LineNumberArea(this);
    this->lineNumberArea->setContentsMargins(0, 0, 0, 0);
    //this->lineNumberArea->setMinimumWidth(20);

    // Setting the CoreEdit background
    QString color(Utilities::getStylesheetValue()->value("@color01").toString());
    QPalette p = palette();
    p.setColor(QPalette::Text, "#B9A388");  //Text color set to white.
    p.setColor(QPalette::Active, QPalette::Base, color);  //Active base color black.
    p.setColor(QPalette::Inactive, QPalette::Base,color); //Inactive base color black.
    setPalette(p);

    //Connecting to slots and signals
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);   //Set the line number area.
    //highlightCurrentLine();         //Highlight the current line.
}

int coreedit::lineNumberAreaWidth()
{
    int digits = 2;
    int max = qMax(1, blockCount());
    while (max >= 100) {
        max /= 10;
        ++digits;
    }
    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
    return space;
}

void coreedit::updateLineNumberAreaWidth(int /*newBlockCount*/)
{
    setViewportMargins(lineNumberAreaWidth() + 2, 0, 0, 0);
}

void coreedit::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void coreedit::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    // Geometry of the line number area
    lineNumberArea->setGeometry(QRect(cr.left() - 2, cr.top(), lineNumberAreaWidth() + 3, cr.height() + 2));
}

void coreedit::highlightCurrentLine()
{
//    QList<QTextEdit::ExtraSelection> extraSelections;

//    if (!isReadOnly()) {
//        QTextEdit::ExtraSelection selection;

//        QColor lineColor = QColor(Qt::gray).lighter(100);

//        selection.format.setBackground(lineColor);
//        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
//        selection.cursor = textCursor();
//        selection.cursor.clearSelection();
//        extraSelections.append(selection);
//    }

//    setExtraSelections(extraSelections);
}

void coreedit::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QColor(Utilities::getStylesheetValue()->value("@color06").toString()));

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Utilities::getStylesheetValue()->value("@color07").toString());
            painter.drawText(-1, top, lineNumberArea->width() + 3, fontMetrics().height() + 2, Qt::AlignRight, number + " ");
        }

        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(blockBoundingRect(block).height());
        ++blockNumber;
    }
}
