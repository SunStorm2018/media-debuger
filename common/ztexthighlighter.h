#ifndef ZTEXTHIGHLIGHTER_H
#define ZTEXTHIGHLIGHTER_H

#include <QObject>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QColor>
#include <QRegularExpression>
#include <QList>

class ZTextHighlighter : public QObject
{
    Q_OBJECT

public:
    explicit ZTextHighlighter(QPlainTextEdit *parent = nullptr);
    ~ZTextHighlighter();

    // 高亮搜索
    void highlight(const QString &searchText);
    void highlight(const QString &searchText, const QColor &backgroundColor, const QColor &textColor = Qt::black);

    // 清除高亮
    void clearHighlight();

    // 导航功能
    void gotoNextHighlight();
    void gotoPreviousHighlight();
    void gotoFirstHighlight();
    void gotoLastHighlight();

    // 设置选项
    void setCaseSensitive(bool caseSensitive);
    void setWholeWord(bool wholeWord);
    void setUseRegex(bool useRegex);
    void setHighlightColor(const QColor &backgroundColor, const QColor &textColor = Qt::black);

    // 状态获取
    int highlightCount() const;
    int currentHighlightIndex() const;
    bool hasHighlights() const;
    QString currentSearchText() const;

    // 高亮所有匹配项，返回匹配数量
    int highlightAll(const QString &searchText);

signals:
    void highlightCountChanged(int count);
    void currentHighlightChanged(int index);
    void searchTextNotFound(const QString &searchText);

private:
    void applyHighlightFormat(QTextCursor &cursor);
    bool isValidSearchText(const QString &text) const;
    QTextDocument::FindFlags getFindFlags() const;

private:
    QPlainTextEdit *m_textEdit;
    QList<QTextCursor> m_highlightedCursors;
    QTextCharFormat m_highlightFormat;
    QString m_currentSearchText;
    bool m_caseSensitive;
    bool m_wholeWord;
    bool m_useRegex;
    int m_currentIndex;
};

#endif // ZTEXTHIGHLIGHTER_H
