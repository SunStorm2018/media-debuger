// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef TABLEHEADERMANAGER_H
#define TABLEHEADERMANAGER_H

#include <QObject>
#include <QHeaderView>
#include <QMenu>
#include <QSettings>
#include <QHash>
#include <QScrollBar>
#include <QLabel>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>

#include "common.h"

class MultiSelectMenu : public QMenu
{
    Q_OBJECT

public:
    explicit MultiSelectMenu(QWidget *parent = nullptr) : QMenu(parent) {}

protected:
    void mouseReleaseEvent(QMouseEvent *event) override
    {
        QAction *action = actionAt(event->pos());
        if (action && action->isCheckable()) {
            action->setChecked(!action->isChecked());
            emit action->toggled(action->isChecked());
            emit action->triggered(action->isChecked());
            return;
        }
        QMenu::mouseReleaseEvent(event);
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        if (event->key() == Qt::Key_Escape) {
            close();
            return;
        }
        QMenu::keyPressEvent(event);
    }

    bool event(QEvent *event) override
    {
        if (event->type() == QEvent::Leave) {
            return true;
        }
        return QMenu::event(event);
    }
};

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

signals:
    void headerToggleVisiable(int index, bool visiable);

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
