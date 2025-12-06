// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef ZTEXTEDITOR_H
#define ZTEXTEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

class ZLineNumberArea;

class ZTextEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    ZTextEditor(QWidget *parent = nullptr);

    int lineNumberAreaWidth();
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    
    // Context menu interface
    void addContextMenu(QMenu *menu);
    void addContextAction(QAction *action);
    void addContextSeparator();
    
    // Find action by object name
    QAction *findContextAction(const QString &objectName);

    QAction *findContextActionByText(const QString &actionText);
    
    // Get all context menu actions
    QList<QAction *> getContextActions();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();
    
    // Context menu slot functions
    void undo();
    void redo();
    void cut();
    void copy();
    void paste();
    void selectAll();
    void deleteSelected();

private:
    ZLineNumberArea *lineNumberArea;
    
    // Context menu related
    QMenu *m_contextMenu;
    QAction *m_undoAction;
    QAction *m_redoAction;
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;
    QAction *m_selectAllAction;
    QAction *m_deleteAction;
    
    void setupContextMenu();
    void updateContextMenuActions();
};

class ZLineNumberArea : public QWidget
{
public:
    ZLineNumberArea(ZTextEditor *editor) : QWidget(editor), zEditor(editor) {}

    QSize sizeHint() const override {
        return QSize(zEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        zEditor->lineNumberAreaPaintEvent(event);
    }

private:
    ZTextEditor *zEditor;
};

#endif // ZTEXTEDITOR_H
    
