#include "tableheadermanager.h"
#include <QSettings>
#include <QDebug>
#include <QScrollBar>
#include <QLabel>
#include <QTableView>

TableHeaderManager::TableHeaderManager(QHeaderView *horizontalHeader, QHeaderView *verticalHeader,QObject *parent)
    : QObject(parent), m_horizontalHeader(horizontalHeader), m_verticalHeader(verticalHeader),
    m_totalCountLabel(nullptr), m_showVerticalHeader(true)
{
    if (m_horizontalHeader) {
        m_horizontalHeader->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_horizontalHeader, &QHeaderView::customContextMenuRequested,
                this, &TableHeaderManager::onHeaderContextMenuRequested);

        // Enable movable headers by default
        m_horizontalHeader->setSectionsMovable(true);

        // Setup total count label
        setupTotalCountLabel();
    }

    if (verticalHeader) {
        verticalHeader->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(verticalHeader, &QHeaderView::customContextMenuRequested,
                this, &TableHeaderManager::onHeaderContextMenuRequested);
    }
}

TableHeaderManager::~TableHeaderManager()
{
    saveState();

    if (m_totalCountLabel) {
        delete m_totalCountLabel;
    }
}

void TableHeaderManager::setObjectName(const QString &name)
{
    m_objectName = name;
}

QString TableHeaderManager::objectName() const
{
    return m_objectName;
}

void TableHeaderManager::enableHeaderContextMenu(bool enable)
{
    if (m_horizontalHeader) {
        m_horizontalHeader->setContextMenuPolicy(enable ? Qt::CustomContextMenu : Qt::NoContextMenu);
    }
}

void TableHeaderManager::saveState()
{
    if (m_horizontalHeader && !m_objectName.isEmpty()) {
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        settings.beginGroup(WINDOWHEAD_GROUP);
        settings.setValue(m_objectName, m_horizontalHeader->saveState());
        settings.endGroup();
        settings.sync();
    }
}

void TableHeaderManager::restoreState()
{
    if (m_horizontalHeader && !m_objectName.isEmpty()) {
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        settings.beginGroup(WINDOWHEAD_GROUP);
        if (settings.contains(m_objectName)) {
            QByteArray columnOrder = settings.value(m_objectName).toByteArray();
            m_horizontalHeader->restoreState(columnOrder);
        }
        settings.endGroup();
    }
}

void TableHeaderManager::onHeaderContextMenuRequested(const QPoint &pos)
{
    if (!m_horizontalHeader || !m_horizontalHeader->model()) return;

    QMenu menu;
    m_actionToColumnMap.clear();

    QAction *showAllAction = menu.addAction(tr("Show All"));
    QAction *toggleVerticalHeaderAction = menu.addAction(tr("Hide Vertical Header"));
    toggleVerticalHeaderAction->setCheckable(true);
    toggleVerticalHeaderAction->setChecked(m_showVerticalHeader);

    QAction *toggleTotalCountAction = menu.addAction(tr("Hide Total Count"));
    toggleTotalCountAction->setCheckable(true);
    toggleTotalCountAction->setChecked(m_totalCountLabel && m_totalCountLabel->isVisible());

    menu.addSeparator();
    menu.addSeparator();

    connect(showAllAction, &QAction::triggered, this, &TableHeaderManager::showAllColumns);

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
        connect(columnAction, &QAction::triggered, this, &TableHeaderManager::toggleColumnVisibility);
    }

    menu.exec(m_horizontalHeader->mapToGlobal(pos));
}

void TableHeaderManager::setTotalCountVisible(bool visible)
{
    if (m_totalCountLabel) {
        m_totalCountLabel->setVisible(visible);
    }
}

void TableHeaderManager::updateTotalCount(int count)
{
    if (m_totalCountLabel) {
        m_totalCountLabel->setText(QString("Total: %1").arg(count));
    }
}

void TableHeaderManager::setupTotalCountLabel()
{
    if (!m_horizontalHeader) return;

    QTableView *tableView = qobject_cast<QTableView*>(m_horizontalHeader->parentWidget());
    if (!tableView) return;

    m_totalCountLabel = new QLabel(tableView);
    m_totalCountLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_totalCountLabel->setStyleSheet("QLabel { padding: 2px; }");
    updateTotalCount(0);
}

void TableHeaderManager::toggleColumnVisibility()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action || !m_actionToColumnMap.contains(action)) return;

    int logicalIndex = m_actionToColumnMap[action];
    if (action->isChecked()) {
        m_horizontalHeader->showSection(logicalIndex);
    } else {
        m_horizontalHeader->hideSection(logicalIndex);
    }
}

void TableHeaderManager::showAllColumns()
{
    if (m_horizontalHeader) {
        for (int i = 0; i < m_horizontalHeader->count(); ++i) {
            m_horizontalHeader->showSection(i);
        }
    }
}
