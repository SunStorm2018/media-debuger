#include "ztexthighlighter.h"
#include <QDebug>

ZTextHighlighter::ZTextHighlighter(QPlainTextEdit *parent)
    : QObject(parent)
    , m_textEdit(parent)
    , m_caseSensitive(false)
    , m_wholeWord(false)
    , m_useRegex(false)
    , m_currentIndex(-1)
{
    // 设置默认高亮格式
    m_highlightFormat.setBackground(QColor(255, 255, 100)); // 浅黄色背景
    m_highlightFormat.setForeground(Qt::black);            // 黑色文字
}

ZTextHighlighter::~ZTextHighlighter()
{
    clearHighlight();
}

void ZTextHighlighter::highlight(const QString &searchText)
{
    highlight(searchText, m_highlightFormat.background().color(), m_highlightFormat.foreground().color());
}

void ZTextHighlighter::highlight(const QString &searchText, const QColor &backgroundColor, const QColor &textColor)
{
    if (!m_textEdit || !isValidSearchText(searchText)) {
        return;
    }

    // 保存当前搜索文本
    m_currentSearchText = searchText;

    // 清除旧的高亮
    clearHighlight();

    // 设置高亮格式
    m_highlightFormat.setBackground(backgroundColor);
    m_highlightFormat.setForeground(textColor);

    QTextCursor originalCursor = m_textEdit->textCursor();
    QTextCursor cursor(m_textEdit->document());
    cursor.movePosition(QTextCursor::Start);

    m_textEdit->setTextCursor(cursor);

    int foundCount = 0;
    bool found = false;

    if (m_useRegex) {
        // 使用正则表达式搜索
        QRegularExpression regex(searchText, m_caseSensitive ? QRegularExpression::NoPatternOption
                                                             : QRegularExpression::CaseInsensitiveOption);
        if (!regex.isValid()) {
            qWarning() << "Invalid regex pattern:" << regex.errorString();
            emit searchTextNotFound(searchText);
            return;
        }

        QRegularExpressionMatchIterator it = regex.globalMatch(m_textEdit->toPlainText());
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            QTextCursor foundCursor(m_textEdit->document());
            foundCursor.setPosition(match.capturedStart());
            foundCursor.setPosition(match.capturedEnd(), QTextCursor::KeepAnchor);

            foundCursor.mergeCharFormat(m_highlightFormat);
            m_highlightedCursors.append(foundCursor);
            foundCount++;
        }
    } else {
        // 普通文本搜索
        QTextDocument::FindFlags flags = getFindFlags();

        while (m_textEdit->find(searchText, flags)) {
            QTextCursor foundCursor = m_textEdit->textCursor();
            applyHighlightFormat(foundCursor);
            m_highlightedCursors.append(foundCursor);
            foundCount++;
            found = true;
        }
    }

    // 恢复原始光标位置
    m_textEdit->setTextCursor(originalCursor);

    if (foundCount > 0) {
        m_currentIndex = 0;
        emit highlightCountChanged(foundCount);
        emit currentHighlightChanged(0);
    } else {
        m_currentIndex = -1;
        emit searchTextNotFound(searchText);
    }
}

void ZTextHighlighter::clearHighlight()
{
    if (!m_textEdit) {
        return;
    }

    // 清除所有存储的高亮光标
    QTextCharFormat plainFormat;
    for (QTextCursor &cursor : m_highlightedCursors) {
        cursor.mergeCharFormat(plainFormat);
    }

    // 重置整个文本编辑器的格式，确保没有残留的高亮样式
    QTextCursor cursor(m_textEdit->document());
    cursor.select(QTextCursor::Document);
    cursor.setCharFormat(plainFormat);
    cursor.clearSelection();

    m_highlightedCursors.clear();
    m_currentIndex = -1;
    m_currentSearchText.clear();

    emit highlightCountChanged(0);
    emit currentHighlightChanged(-1);
}

void ZTextHighlighter::gotoNextHighlight()
{
    if (m_highlightedCursors.isEmpty()) {
        return;
    }

    m_currentIndex = (m_currentIndex + 1) % m_highlightedCursors.size();
    m_textEdit->setTextCursor(m_highlightedCursors[m_currentIndex]);
    m_textEdit->ensureCursorVisible();

    emit currentHighlightChanged(m_currentIndex);
}

void ZTextHighlighter::gotoPreviousHighlight()
{
    if (m_highlightedCursors.isEmpty()) {
        return;
    }

    m_currentIndex = (m_currentIndex - 1 + m_highlightedCursors.size()) % m_highlightedCursors.size();
    m_textEdit->setTextCursor(m_highlightedCursors[m_currentIndex]);
    m_textEdit->ensureCursorVisible();

    emit currentHighlightChanged(m_currentIndex);
}

void ZTextHighlighter::gotoFirstHighlight()
{
    if (m_highlightedCursors.isEmpty()) {
        return;
    }

    m_currentIndex = 0;
    m_textEdit->setTextCursor(m_highlightedCursors.first());
    m_textEdit->ensureCursorVisible();

    emit currentHighlightChanged(0);
}

void ZTextHighlighter::gotoLastHighlight()
{
    if (m_highlightedCursors.isEmpty()) {
        return;
    }

    m_currentIndex = m_highlightedCursors.size() - 1;
    m_textEdit->setTextCursor(m_highlightedCursors.last());
    m_textEdit->ensureCursorVisible();

    emit currentHighlightChanged(m_currentIndex);
}

void ZTextHighlighter::setCaseSensitive(bool caseSensitive)
{
    m_caseSensitive = caseSensitive;
}

void ZTextHighlighter::setWholeWord(bool wholeWord)
{
    m_wholeWord = wholeWord;
}

void ZTextHighlighter::setUseRegex(bool useRegex)
{
    m_useRegex = useRegex;
}

void ZTextHighlighter::setHighlightColor(const QColor &backgroundColor, const QColor &textColor)
{
    m_highlightFormat.setBackground(backgroundColor);
    m_highlightFormat.setForeground(textColor);
}

int ZTextHighlighter::highlightCount() const
{
    return m_highlightedCursors.size();
}

int ZTextHighlighter::currentHighlightIndex() const
{
    return m_currentIndex;
}

bool ZTextHighlighter::hasHighlights() const
{
    return !m_highlightedCursors.isEmpty();
}

QString ZTextHighlighter::currentSearchText() const
{
    return m_currentSearchText;
}

int ZTextHighlighter::highlightAll(const QString &searchText)
{
    highlight(searchText);
    return highlightCount();
}

void ZTextHighlighter::applyHighlightFormat(QTextCursor &cursor)
{
    cursor.mergeCharFormat(m_highlightFormat);
}

bool ZTextHighlighter::isValidSearchText(const QString &text) const
{
    return !text.trimmed().isEmpty();
}

QTextDocument::FindFlags ZTextHighlighter::getFindFlags() const
{
    QTextDocument::FindFlags flags;
    if (m_caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    if (m_wholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }
    return flags;
}
