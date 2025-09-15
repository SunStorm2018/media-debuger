#ifndef MULTICOLUMNSEARCHPROXYMODEL_H
#define MULTICOLUMNSEARCHPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QStringList>
#include <QRegularExpression>

class MultiColumnSearchProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit MultiColumnSearchProxyModel(QObject *parent = nullptr);

    // Search configuration
    void setSearchText(const QString &text);
    void setSearchColumns(const QStringList &columnNames);
    void setSearchColumns(const QList<int> &columnIndices);
    
    // Match control options
    void setCaseSensitive(bool caseSensitive);
    void setMatchWholeWords(bool wholeWords);
    void setUseRegularExpression(bool useRegex);
    
    // Filter configuration
    void setSearchMode(bool searchInSelectedColumns);
    void resetFilters();

    // Getters
    QString getSearchText() const { return m_searchText; }
    QStringList getSearchColumns() const { return m_searchColumnNames; }
    QList<int> getSearchColumnIndices() const { return m_searchColumnIndices; }
    bool isCaseSensitive() const { return m_caseSensitive; }
    bool isMatchWholeWords() const { return m_wholeWords; }
    bool isUseRegularExpression() const { return m_useRegex; }
    bool isSearchInSelectedColumns() const { return m_searchInSelectedColumns; }

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    void updateRegularExpression();
    bool matchesInColumn(const QString &text, int column) const;
    QString escapeRegexSpecialChars(const QString &text) const;

private:
    QString m_searchText;
    QStringList m_searchColumnNames;
    QList<int> m_searchColumnIndices;
    
    // Match control options
    bool m_caseSensitive;
    bool m_wholeWords;
    bool m_useRegex;
    bool m_searchInSelectedColumns;
    
    // Compiled regular expression for performance
    QRegularExpression m_regex;
};

#endif // MULTICOLUMNSEARCHPROXYMODEL_H