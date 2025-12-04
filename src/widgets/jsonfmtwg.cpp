// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "jsonfmtwg.h"
#include "ui_jsonfmtwg.h"

JsonFormatWG::JsonFormatWG(QWidget *parent)
    : BaseFormatWG(parent)
    , ui(new Ui::JsonFormatWG)
{
    ui->setupUi(this);

    m_model = new QJsonModel(this);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setRecursiveFilteringEnabled(true);
    m_proxyModel->setFilterRole(Qt::DisplayRole);

    ui->treeView->setModel(m_proxyModel);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_contextMenu = new QMenu(this);
    
    // Create copy submenu
    QMenu *copyMenu = m_contextMenu->addMenu(tr("Copy"));
    copyMenu->addAction(tr("Value"), this, &JsonFormatWG::copyValue);
    copyMenu->addAction(tr("Key"), this, &JsonFormatWG::copyKey);
    copyMenu->addAction(tr("Key-Value"), this, &JsonFormatWG::copyKeyValue);
    copyMenu->addSeparator();
    copyMenu->addAction(tr("All Data"), this, &JsonFormatWG::copyAllData);
    
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(tr("Search"), this, &JsonFormatWG::toggleSearch);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(tr("Expand All"), this, &JsonFormatWG::expandAll);
    m_contextMenu->addAction(tr("Collapse All"), this, &JsonFormatWG::collapseAll);

    ui->treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeView, &QTreeView::customContextMenuRequested, this, &JsonFormatWG::showContextMenu);

    m_searchWG = new SearchWG(this);
    m_searchWG->setWindowTitle(tr("JSON Search"));

    // Configure to show only the required group boxes for JsonFormatWG
    auto requiredBoxes = SearchWG::MatchControl | SearchWG::Operation;
    m_searchWG->setVisibleGroupBoxes(requiredBoxes);

    ui->verticalLayout->addWidget(m_searchWG);
    m_searchWG->setVisible(false);

    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
    connect(shortcut, &QShortcut::activated, this, [this]() {
        m_searchWG->setVisible(!m_searchWG->isVisible());
    });

    connect(m_searchWG, &SearchWG::searchReady, this, &JsonFormatWG::on_searchReady);
    connect(m_searchWG, &SearchWG::searchTextChanged, this, &JsonFormatWG::on_searchTextChanged);
    connect(m_searchWG, &SearchWG::searchClear, this, &JsonFormatWG::on_searchClear);
}

JsonFormatWG::~JsonFormatWG()
{
    delete ui;
}

void JsonFormatWG::setControlMargin(const int &top, const int &bottom, const int &left, const int &right)
{
    ui->verticalLayout->setContentsMargins(left, top, right, bottom);
}

bool JsonFormatWG::loadJson(const QByteArray &json)
{
    bool res = m_model->loadJson(json);
    ui->treeView->expandAll();
    return res;
}

void JsonFormatWG::showContextMenu(const QPoint &pos)
{
    QModelIndex index = ui->treeView->indexAt(pos);
    if (index.isValid() || !ui->treeView->selectionModel()->selectedIndexes().isEmpty()) {
        m_contextMenu->exec(ui->treeView->viewport()->mapToGlobal(pos));
    }
}

void JsonFormatWG::copyValue()
{
    QModelIndexList selectedIndexes = ui->treeView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) return;

    QStringList values;
    for (const QModelIndex &proxyIndex : selectedIndexes) {
        if (proxyIndex.column() == 0) {
            QString value = getValueForIndex(proxyIndex);
            if (!value.isEmpty()) {
                values.append(value);
            }
        }
    }

    if (!values.isEmpty()) {
        QApplication::clipboard()->setText(values.join("\n"));
    }
}

void JsonFormatWG::copyKeyValue()
{
    QModelIndexList selectedIndexes = ui->treeView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) return;

    QStringList keyValues;
    for (const QModelIndex &proxyIndex : selectedIndexes) {
        if (proxyIndex.column() == 0) {
            QString key = getKeyForIndex(proxyIndex);
            QString value = getValueForIndex(proxyIndex);
            if (!key.isEmpty() && !value.isEmpty()) {
                keyValues.append(QString("%1: %2").arg(key).arg(value));
            } else if (!key.isEmpty()) {
                keyValues.append(key);
            } else if (!value.isEmpty()) {
                keyValues.append(value);
            }
        }
    }

    if (!keyValues.isEmpty()) {
        QApplication::clipboard()->setText(keyValues.join("\n"));
    }
}

void JsonFormatWG::copyKey()
{
    QModelIndexList selectedIndexes = ui->treeView->selectionModel()->selectedIndexes();
    if (selectedIndexes.isEmpty()) return;

    QStringList keys;
    for (const QModelIndex &proxyIndex : selectedIndexes) {
        if (proxyIndex.column() == 0) {
            QString key = getKeyForIndex(proxyIndex);
            if (!key.isEmpty()) {
                keys.append(key);
            }
        }
    }

    if (!keys.isEmpty()) {
        QApplication::clipboard()->setText(keys.join("\n"));
    }
}

void JsonFormatWG::copyAllData()
{
    QByteArray jsonData = m_model->json(false); // false表示不压缩，格式化输出
    if (!jsonData.isEmpty()) {
        QApplication::clipboard()->setText(QString::fromUtf8(jsonData));
    }
}

void JsonFormatWG::expandAll()
{
    ui->treeView->expandAll();
}

void JsonFormatWG::collapseAll()
{
    ui->treeView->collapseAll();
}

void JsonFormatWG::toggleSearch()
{
    if (m_searchWG) {
        m_searchWG->setVisible(!m_searchWG->isVisible());
        if (m_searchWG->isVisible()) {
            m_searchWG->setFocus();
        }
    }
}

QJsonTreeItem* JsonFormatWG::getItemForIndex(const QModelIndex &proxyIndex)
{
    QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    if (sourceIndex.isValid()) {
        return static_cast<QJsonTreeItem*>(sourceIndex.internalPointer());
    }
    return nullptr;
}

QString JsonFormatWG::getKeyForIndex(const QModelIndex &proxyIndex)
{
    QJsonTreeItem* item = getItemForIndex(proxyIndex);
    if (item) {
        return item->key();
    }
    return QString();
}

QString JsonFormatWG::getValueForIndex(const QModelIndex &proxyIndex)
{
    QJsonTreeItem* item = getItemForIndex(proxyIndex);
    if (item) {
        QVariant value = item->value();
        if (value.isValid()) {
            switch (item->type()) {
            case QJsonValue::Bool:
                return value.toBool() ? "true" : "false";
            case QJsonValue::Double:
                return QString::number(value.toDouble());
            case QJsonValue::String:
                return value.toString();
            case QJsonValue::Array:
                return "[Array]";
            case QJsonValue::Object:
                return "[Object]";
            case QJsonValue::Null:
                return "null";
            default:
                return value.toString();
            }
        }
    }
    return QString();
}

void JsonFormatWG::on_searchReady()
{
    QString searchText = m_searchWG->getSearchText().trimmed();
    if (searchText.isEmpty()) {
        m_searchWG->setSearchStatus(tr("Search text is empty"));
        return;
    }

    // Check if there is content to search
    if (m_model->rowCount() == 0) {
        m_searchWG->setSearchStatus(tr("No content to search"));
        return;
    }

    if (m_searchWG->isCaseSensitive()) {
        m_proxyModel->setFilterCaseSensitivity(Qt::CaseSensitive);
    } else {
        m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    }

    m_proxyModel->setFilterKeyColumn(-1); // Search all columns
    m_proxyModel->setFilterRole(Qt::DisplayRole);

    if (m_searchWG->isUseRegularExpression()) {
        if (m_searchWG->isMatchWholewords()) {
            // For whole word matching with regex, we need to adjust the pattern
            searchText = QString("\\b%1\\b").arg(searchText);
        }
        QT_SET_FILTER_REGEXP(m_proxyModel, searchText);
    } else {
        if (m_searchWG->isMatchWholewords()) {
            // For whole word matching without regex, we need a different approach
            // Since QSortFilterProxyModel doesn't support whole word matching directly,
            // we'll use a regex pattern for this case too
            searchText = QString("\\b%1\\b").arg(QRegularExpression::escape(searchText));
            QT_SET_FILTER_REGEXP(m_proxyModel, searchText);
        } else {
            m_proxyModel->setFilterFixedString(searchText);
        }
    }

    // Update search status
    int visibleCount = 0;
    int totalCount = 0;
    countVisibleAndTotalItems(m_proxyModel, QModelIndex(), visibleCount, totalCount);

    if (visibleCount > 0) {
        m_searchWG->setSearchStatus(tr("Found %1 of %2 items").arg(visibleCount).arg(totalCount));
        ui->treeView->expandAll();
    } else {
        m_searchWG->setSearchStatus(tr("No items found"));
    }
}

void JsonFormatWG::on_searchTextChanged(const QString &text)
{
    if (text.isEmpty()) {
        m_proxyModel->setFilterFixedString("");
        m_searchWG->setSearchStatus("");
    }
}

void JsonFormatWG::on_searchClear()
{
    m_proxyModel->setFilterFixedString("");
    m_searchWG->setSearchText("");
    m_searchWG->setSearchStatus("");
    ui->treeView->expandAll();
}

void JsonFormatWG::countVisibleAndTotalItems(QAbstractItemModel *model, const QModelIndex &parent, int &visibleCount, int &totalCount)
{
    for (int i = 0; i < model->rowCount(parent); ++i) {
        QModelIndex index = model->index(i, 0, parent);
        totalCount++;

        // Check if the item is visible in the proxy model
        bool itemVisible = false;
        if (model == m_proxyModel) {
            // If we're already working with the proxy model
            itemVisible = true;
        } else {
            // If we're working with the source model, check if it has a mapping in the proxy
            QModelIndex proxyIndex = m_proxyModel->mapFromSource(index);
            itemVisible = proxyIndex.isValid();
        }

        // Check children
        int childVisibleCount = 0;
        int childTotalCount = 0;
        if (model->hasChildren(index)) {
            QAbstractItemModel *childModel = model;
            QModelIndex childParent = index;

            // If we're using the source model, we need to work with the proxy model for children
            if (model == m_model) {
                QModelIndex proxyIndex = m_proxyModel->mapFromSource(index);
                if (proxyIndex.isValid()) {
                    childModel = m_proxyModel;
                    childParent = proxyIndex;
                } else {
                    childModel = nullptr; // Skip children if parent is not visible
                }
            }

            if (childModel) {
                countVisibleAndTotalItems(childModel, childParent, childVisibleCount, childTotalCount);
            }
        }

        if (itemVisible) {
            visibleCount += 1 + childVisibleCount;
        }
        totalCount += childTotalCount;
    }
}
