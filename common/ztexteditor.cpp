#include "ztexteditor.h"
#include <QPainter>
#include <QTextBlock>
#include <QResizeEvent>

// GitHub-style color definitions
const QColor LINE_NUMBER_BG = QColor(247, 247, 247);       // Line number area background
const QColor LINE_NUMBER_TEXT = QColor(153, 153, 153);      // Line number text color
const QColor CURRENT_LINE_BG = QColor(247, 247, 247);       // Current line background
const QColor CURRENT_LINE_NUMBER = QColor(55, 118, 171);    // Current line number color
const QColor EDITOR_BG = QColor(255, 255, 255);             // Editor background
const int LINE_NUMBER_RIGHT_MARGIN = 8;                     // Line number right margin

ZTextEditor::ZTextEditor(QWidget *parent)
    : QPlainTextEdit(parent)
{
    lineNumberArea = new ZLineNumberArea(this);

    // Set editor background color to white (GitHub style)
    setStyleSheet(QString("background-color: rgb(%1, %2, %3);")
                      .arg(EDITOR_BG.red()).arg(EDITOR_BG.green()).arg(EDITOR_BG.blue()));

    // Connect signals and slots
    connect(this, &ZTextEditor::blockCountChanged,
            this, &ZTextEditor::updateLineNumberAreaWidth);
    connect(this, &ZTextEditor::updateRequest,
            this, &ZTextEditor::updateLineNumberArea);
    connect(this, &ZTextEditor::cursorPositionChanged,
            this, &ZTextEditor::highlightCurrentLine);
    // Listen for font changes to ensure line numbers match text font
    connect(this, &::ZTextEditor::textChanged,
            this, &ZTextEditor::updateLineNumberAreaWidth);

    updateLineNumberAreaWidth();
    highlightCurrentLine();
}

int ZTextEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        digits++;
    }
    // Add right margin when calculating width
    return 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits + LINE_NUMBER_RIGHT_MARGIN;
}

void ZTextEditor::updateLineNumberAreaWidth()
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ZTextEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth();
}

void ZTextEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void ZTextEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        // Current line background color (GitHub light gray)
        selection.format.setBackground(CURRENT_LINE_BG);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }
    setExtraSelections(extraSelections);
}

void ZTextEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    // Line number area background color (GitHub-style light gray)
    painter.fillRect(event->rect(), LINE_NUMBER_BG);

    // Use the same font as the editor for line numbers to ensure height matching
    painter.setFont(font());

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    // Get actual top position of text block (considering font height)
    int top = (int)blockBoundingGeometry(block).translated(contentOffset()).top();
    // Calculate text block height (based on actual font metrics)
    int blockHeight = fontMetrics().height();
    int bottom = top + blockHeight;

    int currentLine = textCursor().blockNumber();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            // Highlight current line number (GitHub blue)
            if (blockNumber == currentLine) {
                painter.setPen(CURRENT_LINE_NUMBER);
                painter.setFont(QFont(font().family(), font().pointSize(), QFont::Medium));
            } else {
                painter.setPen(LINE_NUMBER_TEXT);
                painter.setFont(font());
            }
            // Draw line numbers, accounting for right margin
            painter.drawText(0, top, lineNumberArea->width() - LINE_NUMBER_RIGHT_MARGIN,
                             blockHeight, Qt::AlignRight | Qt::AlignTop, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + blockHeight;
        blockNumber++;
    }
}
