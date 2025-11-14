// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef ZTEXTEDITOR_H
#define ZTEXTEDITOR_H

#include <QPlainTextEdit>
#include <QWidget>

class ZLineNumberArea;

class ZTextEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    ZTextEditor(QWidget *parent = nullptr);

    int lineNumberAreaWidth();
    void lineNumberAreaPaintEvent(QPaintEvent *event);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth();
    void updateLineNumberArea(const QRect &rect, int dy);
    void highlightCurrentLine();

private:
    ZLineNumberArea *lineNumberArea;
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
    
