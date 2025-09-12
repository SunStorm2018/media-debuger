#ifndef TABLEHEADERMANAGER_H
#define TABLEHEADERMANAGER_H

#include <QObject>
#include <QHeaderView>
#include <QMenu>
#include <QSettings>
#include <QHash>
#include <QScrollBar>
#include <QLabel>

#include <common/common.h>

class TableHeaderManager : public QObject
{
    Q_OBJECT

public:
    explicit TableHeaderManager(QHeaderView *horizontalHeader, QHeaderView *verticalHeader, QObject *parent = nullptr);
    ~TableHeaderManager();

    void setObjectName(const QString &name);
    QString objectName() const;

    void enableHeaderContextMenu(bool enable = true);
    void saveState();
    void restoreState();

    void setTotalCountVisible(bool visible);
    void updateTotalCount(int count);

private slots:
    void onHeaderContextMenuRequested(const QPoint &pos);
    void toggleColumnVisibility();
    void showAllColumns();

private:
    void setupTotalCountLabel();

private:
    QHeaderView *m_horizontalHeader;
    QHeaderView *m_verticalHeader;
    QString m_objectName;
    QHash<QAction*, int> m_actionToColumnMap;
    QLabel *m_totalCountLabel;
    bool m_showVerticalHeader;
};

#endif // TABLEHEADERMANAGER_H
