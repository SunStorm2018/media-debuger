// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "ztableheadermanager.h"
#include <QSettings>
#include <QDebug>
#include <QScrollBar>
#include <QLabel>
#include <QTableView>

ZTableHeaderManager::ZTableHeaderManager(QHeaderView *horizontalHeader, QHeaderView *verticalHeader,QObject *parent)
    : QObject(parent), m_horizontalHeader(horizontalHeader), m_verticalHeader(verticalHeader),
    m_totalCountLabel(nullptr), m_showVerticalHeader(true)
{
    if (m_horizontalHeader) {
        m_horizontalHeader->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_horizontalHeader, &QHeaderView::customContextMenuRequested,
                this, &ZTableHeaderManager::onHeaderContextMenuRequested);

        // Enable movable headers by default
        m_horizontalHeader->setSectionsMovable(true);

        // Setup total count label
        setupTotalCountLabel();
    }

    if (verticalHeader) {
        verticalHeader->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(verticalHeader, &QHeaderView::customContextMenuRequested,
                this, &ZTableHeaderManager::onHeaderContextMenuRequested);
    }
}

ZTableHeaderManager::~ZTableHeaderManager()
{
    saveState();

    if (m_totalCountLabel) {
        delete m_totalCountLabel;
    }
}

void ZTableHeaderManager::setObjectName(const QString &name)
{
    m_objectName = name;
}

QString ZTableHeaderManager::objectName() const
{
    return m_objectName;
}

void ZTableHeaderManager::enableHeaderContextMenu(bool enable)
{
    if (m_horizontalHeader) {
        m_horizontalHeader->setContextMenuPolicy(enable ? Qt::CustomContextMenu : Qt::NoContextMenu);
    }
}

void ZTableHeaderManager::saveState()
{
    if (m_horizontalHeader && !m_objectName.isEmpty()) {
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        settings.beginGroup(WINDOWHEAD_SETTINGS_GROUP);
        settings.setValue(m_objectName, m_horizontalHeader->saveState());
        settings.endGroup();
        settings.sync();
    }
}

void ZTableHeaderManager::restoreState()
{
    if (m_horizontalHeader && !m_objectName.isEmpty()) {
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        settings.beginGroup(WINDOWHEAD_SETTINGS_GROUP);
        if (settings.contains(m_objectName)) {
            QByteArray columnOrder = settings.value(m_objectName).toByteArray();
            m_horizontalHeader->restoreState(columnOrder);
        }
        settings.endGroup();
    }
}

void ZTableHeaderManager::onHeaderContextMenuRequested(const QPoint &pos)
{
    if (!m_horizontalHeader || !m_horizontalHeader->model()) return;

    ZMultiSelectMenu menu;
    menu.setTearOffEnabled(true);  // Enable tear-off menu to keep it open
    menu.setTitle(tr("Column Visibility"));  // Set title for torn-off menu
    m_actionToColumnMap.clear();

    QAction *showAllAction = menu.addAction(tr("Show All"));
    QAction *toggleVerticalHeaderAction = menu.addAction(tr("Hide Vertical Header"));
    toggleVerticalHeaderAction->setCheckable(true);
    toggleVerticalHeaderAction->setChecked(m_showVerticalHeader);

    QAction *toggleTotalCountAction = menu.addAction(tr("Hide Total Count"));
    toggleTotalCountAction->setCheckable(true);
    toggleTotalCountAction->setChecked(m_totalCountLabel && m_totalCountLabel->isVisible());

    menu.addSeparator();

    connect(showAllAction, &QAction::triggered, this, &ZTableHeaderManager::showAllColumns);

    connect(toggleVerticalHeaderAction, &QAction::triggered, [=](bool checked){
        m_showVerticalHeader = !m_showVerticalHeader;
        if (m_verticalHeader) {
            m_verticalHeader->setVisible(m_showVerticalHeader);
            QTableView *tableView = qobject_cast<QTableView*>(m_horizontalHeader->parentWidget());
            if (tableView) {
                tableView->verticalHeader()->update();
            }

            qDebug() << toggleVerticalHeaderAction->text();
            if (checked) {
                toggleVerticalHeaderAction->setText(tr("Show Vertical Header"));
            } else {
                toggleVerticalHeaderAction->setText(tr("Hide Vertical Header"));
            }
        }
    });

    connect(toggleTotalCountAction, &QAction::triggered, [=](bool checked) {
        if (m_totalCountLabel) {
            m_totalCountLabel->setVisible(checked);

            if (checked) {
                toggleTotalCountAction->setText(tr("Show Total Count"));
            } else {
                toggleTotalCountAction->setText(tr("Hide Total Count"));
            }
        }
    });

    for (int logicalIndex = 0; logicalIndex < m_horizontalHeader->count(); ++logicalIndex) {
        QString columnName = m_horizontalHeader->model()->headerData(logicalIndex, Qt::Horizontal).toString();
        if (columnName.isEmpty()) {
            columnName = QString(tr("Column %1")).arg(logicalIndex + 1);
        }

        QAction *columnAction = menu.addAction(columnName);
        columnAction->setCheckable(true);
        columnAction->setChecked(!m_horizontalHeader->isSectionHidden(logicalIndex));

        m_actionToColumnMap[columnAction] = logicalIndex;
        connect(columnAction, &QAction::triggered, this, &ZTableHeaderManager::toggleColumnVisibility);
    }
    
    // Show menu at the cursor position
    menu.exec(m_horizontalHeader->mapToGlobal(pos));
}

void ZTableHeaderManager::setTotalCountVisible(bool visible)
{
    if (m_totalCountLabel) {
        m_totalCountLabel->setVisible(visible);
    }
}

void ZTableHeaderManager::updateTotalCount(int count)
{
    if (m_totalCountLabel) {
        m_totalCountLabel->setText(QString("Total: %1").arg(count));
    }
}

void ZTableHeaderManager::setupTotalCountLabel()
{
    if (!m_horizontalHeader) return;

    QTableView *tableView = qobject_cast<QTableView*>(m_horizontalHeader->parentWidget());
    if (!tableView) return;

    m_totalCountLabel = new QLabel(tableView);
    m_totalCountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_totalCountLabel->setStyleSheet("QLabel { padding: 2px; }");
    updateTotalCount(0);
}

void ZTableHeaderManager::toggleColumnVisibility()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action || !m_actionToColumnMap.contains(action)) return;

    int logicalIndex = m_actionToColumnMap[action];
    if (action->isChecked()) {
        m_horizontalHeader->showSection(logicalIndex);
    } else {
        m_horizontalHeader->hideSection(logicalIndex);
    }

    emit headerToggleVisiable(logicalIndex, action->isChecked());
}

void ZTableHeaderManager::showAllColumns()
{
    if (m_horizontalHeader) {
        for (int i = 0; i < m_horizontalHeader->count(); ++i) {
            m_horizontalHeader->showSection(i);
        }
    }
}