// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "ztexteditor.h"
#include "common.h"
#include <QPainter>
#include <QTextBlock>
#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QClipboard>
#include <QApplication>
#include <QKeyEvent>

// GitHub-style color definitions
const QColor LINE_NUMBER_BG = QColor(247, 247, 247);       // Line number area background
const QColor LINE_NUMBER_TEXT = QColor(153, 153, 153);      // Line number text color
const QColor CURRENT_LINE_BG = QColor(247, 247, 247);       // Current line background
const QColor CURRENT_LINE_NUMBER = QColor(55, 118, 171);    // Current line number color
const QColor EDITOR_BG = QColor(255, 255, 255);             // Editor background
const int LINE_NUMBER_RIGHT_MARGIN = 8;                     // Line number right margin

ZTextEditor::ZTextEditor(QWidget *parent)
    : QPlainTextEdit(parent)
    , m_contextMenu(nullptr)
    , m_undoAction(nullptr)
    , m_redoAction(nullptr)
    , m_cutAction(nullptr)
    , m_copyAction(nullptr)
    , m_pasteAction(nullptr)
    , m_selectAllAction(nullptr)
    , m_deleteAction(nullptr)
{
    lineNumberArea = new ZLineNumberArea(this);

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
    
    // Setup context menu
    setContextMenuPolicy(Qt::DefaultContextMenu);
    setupContextMenu();
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

void ZTextEditor::contextMenuEvent(QContextMenuEvent *event)
{
    updateContextMenuActions();
    m_contextMenu->exec(event->globalPos());
}

void ZTextEditor::setupContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    // Undo
    m_undoAction = new QAction(tr("Undo"), this);
    m_undoAction->setShortcut(QKeySequence::Undo);
    connect(m_undoAction, &QAction::triggered, this, &ZTextEditor::undo);
    m_contextMenu->addAction(m_undoAction);
    
    // Redo
    m_redoAction = new QAction(tr("Redo"), this);
    m_redoAction->setShortcut(QKeySequence::Redo);
    connect(m_redoAction, &QAction::triggered, this, &ZTextEditor::redo);
    m_contextMenu->addAction(m_redoAction);
    
    m_contextMenu->addSeparator();
    
    // Cut
    m_cutAction = new QAction(tr("Cut"), this);
    m_cutAction->setShortcut(QKeySequence::Cut);
    connect(m_cutAction, &QAction::triggered, this, &ZTextEditor::cut);
    m_contextMenu->addAction(m_cutAction);
    
    // Copy
    m_copyAction = new QAction(tr("Copy"), this);
    m_copyAction->setShortcut(QKeySequence::Copy);
    connect(m_copyAction, &QAction::triggered, this, &ZTextEditor::copy);
    m_contextMenu->addAction(m_copyAction);
    
    // Paste
    m_pasteAction = new QAction(tr("Paste"), this);
    m_pasteAction->setShortcut(QKeySequence::Paste);
    connect(m_pasteAction, &QAction::triggered, this, &ZTextEditor::paste);
    m_contextMenu->addAction(m_pasteAction);
    
    // Delete
    m_deleteAction = new QAction(tr("Delete"), this);
    m_deleteAction->setShortcut(QKeySequence::Delete);
    connect(m_deleteAction, &QAction::triggered, this, &ZTextEditor::deleteSelected);
    m_contextMenu->addAction(m_deleteAction);
    
    m_contextMenu->addSeparator();
    
    // Select All
    m_selectAllAction = new QAction(tr("Select All"), this);
    m_selectAllAction->setShortcut(QKeySequence::SelectAll);
    connect(m_selectAllAction, &QAction::triggered, this, &ZTextEditor::selectAll);
    m_contextMenu->addAction(m_selectAllAction);
}

void ZTextEditor::updateContextMenuActions()
{
    // Update undo/redo status
    m_undoAction->setEnabled(document()->isUndoAvailable());
    m_redoAction->setEnabled(document()->isRedoAvailable());
    
    // Update cut/copy/delete status
    bool hasSelection = textCursor().hasSelection();
    m_cutAction->setEnabled(hasSelection && !isReadOnly());
    m_copyAction->setEnabled(hasSelection);
    m_deleteAction->setEnabled(hasSelection && !isReadOnly());
    
    // Update paste status
    m_pasteAction->setEnabled(!isReadOnly() && QApplication::clipboard()->text().length() > 0);
    
    // Update select all status
    m_selectAllAction->setEnabled(document()->characterCount() > 1); // >1 because includes final null character
}

void ZTextEditor::addContextMenu(QMenu *menu)
{
    if (menu && m_contextMenu) {
        m_contextMenu->addMenu(menu);
    }
}

void ZTextEditor::addContextAction(QAction *action)
{
    if (action && m_contextMenu) {
        m_contextMenu->addAction(action);
    }
}

void ZTextEditor::addContextSeparator()
{
    if (m_contextMenu) {
        m_contextMenu->addSeparator();
    }
}

QAction *ZTextEditor::findContextAction(const QString &objectName)
{
    if (!m_contextMenu) {
        return nullptr;
    }
    
    // Use Common utility function
    return Common::findActionByObjectName(m_contextMenu, objectName);
}

QAction *ZTextEditor::findContextActionByText(const QString &actionText)
{
    if (!m_contextMenu) {
        return nullptr;
    }
    
    // Use Common utility function
    return Common::findActionByText(m_contextMenu, actionText);
}

QList<QAction *> ZTextEditor::getContextActions()
{
    if (!m_contextMenu) {
        return QList<QAction *>();
    }
    
    // Use Common utility function
    return Common::getAllActions(m_contextMenu);
}

void ZTextEditor::undo()
{
    QPlainTextEdit::undo();
}

void ZTextEditor::redo()
{
    QPlainTextEdit::redo();
}

void ZTextEditor::cut()
{
    QPlainTextEdit::cut();
}

void ZTextEditor::copy()
{
    QPlainTextEdit::copy();
}

void ZTextEditor::paste()
{
    QPlainTextEdit::paste();
}

void ZTextEditor::selectAll()
{
    QPlainTextEdit::selectAll();
}

void ZTextEditor::deleteSelected()
{
    if (textCursor().hasSelection() && !isReadOnly()) {
        textCursor().removeSelectedText();
    }
}
