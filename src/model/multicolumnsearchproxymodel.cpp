// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "multicolumnsearchproxymodel.h"
#include <QDebug>

MultiColumnSearchProxyModel::MultiColumnSearchProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_caseSensitive(false)
    , m_wholeWords(false)
    , m_useRegex(false)
    , m_searchInSelectedColumns(false)
{
    // Default to case insensitive filtering
    setFilterCaseSensitivity(Qt::CaseInsensitive);
}

void MultiColumnSearchProxyModel::setSearchText(const QString &text)
{
    if (m_searchText != text) {
        m_searchText = text;
        updateRegularExpression();
        invalidateFilter();
    }
}

void MultiColumnSearchProxyModel::setSearchColumns(const QStringList &columnNames)
{
    if (m_searchColumnNames != columnNames) {
        m_searchColumnNames = columnNames;
        
        // Convert column names to indices
        m_searchColumnIndices.clear();
        if (sourceModel()) {
            for (const QString &columnName : columnNames) {
                for (int col = 0; col < sourceModel()->columnCount(); ++col) {
                    QString headerData = sourceModel()->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
                    if (headerData == columnName) {
                        m_searchColumnIndices.append(col);
                        break;
                    }
                }
            }
        }
        
        qDebug() << "Search columns set:" << columnNames << "-> indices:" << m_searchColumnIndices;
        invalidateFilter();
    }
}

void MultiColumnSearchProxyModel::setSearchColumns(const QList<int> &columnIndices)
{
    if (m_searchColumnIndices != columnIndices) {
        m_searchColumnIndices = columnIndices;
        
        // Convert indices to column names
        m_searchColumnNames.clear();
        if (sourceModel()) {
            for (int col : columnIndices) {
                if (col >= 0 && col < sourceModel()->columnCount()) {
                    QString headerData = sourceModel()->headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
                    m_searchColumnNames.append(headerData);
                }
            }
        }
        
        qDebug() << "Search column indices set:" << columnIndices << "-> names:" << m_searchColumnNames;
        invalidateFilter();
    }
}

void MultiColumnSearchProxyModel::setCaseSensitive(bool caseSensitive)
{
    if (m_caseSensitive != caseSensitive) {
        m_caseSensitive = caseSensitive;
        setFilterCaseSensitivity(caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
        updateRegularExpression();
        invalidateFilter();
    }
}

void MultiColumnSearchProxyModel::setMatchWholeWords(bool wholeWords)
{
    if (m_wholeWords != wholeWords) {
        m_wholeWords = wholeWords;
        updateRegularExpression();
        invalidateFilter();
    }
}

void MultiColumnSearchProxyModel::setUseRegularExpression(bool useRegex)
{
    if (m_useRegex != useRegex) {
        m_useRegex = useRegex;
        updateRegularExpression();
        invalidateFilter();
    }
}

void MultiColumnSearchProxyModel::setSearchMode(bool searchInSelectedColumns)
{
    if (m_searchInSelectedColumns != searchInSelectedColumns) {
        m_searchInSelectedColumns = searchInSelectedColumns;
        invalidateFilter();
    }
}

void MultiColumnSearchProxyModel::resetFilters()
{
    m_searchText.clear();
    m_searchColumnNames.clear();
    m_searchColumnIndices.clear();
    m_caseSensitive = false;
    m_wholeWords = false;
    m_useRegex = false;
    m_searchInSelectedColumns = false;
    
    setFilterCaseSensitivity(Qt::CaseInsensitive);
    updateRegularExpression();
    invalidateFilter();
}

bool MultiColumnSearchProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    // If no search text, accept all rows
    if (m_searchText.isEmpty()) {
        return true;
    }
    
    // If no source model, reject
    if (!sourceModel()) {
        return false;
    }
    
    // Determine which columns to search
    QList<int> columnsToSearch;
    
    if (m_searchInSelectedColumns && !m_searchColumnIndices.isEmpty()) {
        // Search only in selected columns
        columnsToSearch = m_searchColumnIndices;
    } else {
        // Search in all columns
        for (int col = 0; col < sourceModel()->columnCount(); ++col) {
            columnsToSearch.append(col);
        }
    }
    
    // Search in the determined columns
    for (int col : columnsToSearch) {
        QModelIndex index = sourceModel()->index(source_row, col, source_parent);
        if (index.isValid()) {
            QString cellData = sourceModel()->data(index, Qt::DisplayRole).toString();
            
            if (matchesInColumn(cellData, col)) {
                return true; // Found match in at least one column
            }
        }
    }
    
    return false; // No match found in any searched column
}

void MultiColumnSearchProxyModel::updateRegularExpression()
{
    if (m_searchText.isEmpty()) {
        m_regex = QRegularExpression();
        return;
    }
    
    QString pattern = m_searchText;
    
    // Escape special regex characters if not using regex mode
    if (!m_useRegex) {
        pattern = escapeRegexSpecialChars(pattern);
    }
    
    // Add word boundaries for whole word matching
    if (m_wholeWords) {
        pattern = QString("\\b%1\\b").arg(pattern);
    }
    
    // Set case sensitivity
    QRegularExpression::PatternOptions options = QRegularExpression::NoPatternOption;
    if (!m_caseSensitive) {
        options |= QRegularExpression::CaseInsensitiveOption;
    }
    
    // Note: OptimizeOnFirstUsageOption was removed in Qt6 as the regex engine is optimized by default
    
    m_regex.setPattern(pattern);
    m_regex.setPatternOptions(options);
    
    if (!m_regex.isValid()) {
        qWarning() << "Invalid regular expression:" << pattern << "Error:" << m_regex.errorString();
        // Fallback to escaped pattern
        m_regex.setPattern(escapeRegexSpecialChars(m_searchText));
        m_regex.setPatternOptions(options);
    }
}

bool MultiColumnSearchProxyModel::matchesInColumn(const QString &text, int column) const
{
    Q_UNUSED(column) // Column-specific logic can be added here if needed
    
    if (m_searchText.isEmpty()) {
        return true;
    }
    
    if (m_useRegex || m_wholeWords) {
        return m_regex.match(text).hasMatch();
    } else {
        // Simple substring search
        return text.contains(m_searchText, m_caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);
    }
}

QString MultiColumnSearchProxyModel::escapeRegexSpecialChars(const QString &text) const
{
    QString result = text;
    
    // Escape regex special characters
    static const QString specialChars = "\\^$.*+?()[]{}|";
    for (const QChar &ch : specialChars) {
        result.replace(ch, QString("\\%1").arg(ch));
    }
    
    return result;
}
