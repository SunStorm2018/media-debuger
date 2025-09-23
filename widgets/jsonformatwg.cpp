#include "jsonformatwg.h"
#include "ui_jsonformatwg.h"

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

bool JsonFormatWG::loadJson(const QByteArray &json)
{
    bool res = m_model->loadJson(json);
    ui->treeView->expandAll();
    return res;
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
        m_proxyModel->setFilterRegExp(searchText);
    } else {
        if (m_searchWG->isMatchWholewords()) {
            // For whole word matching without regex, we need a different approach
            // Since QSortFilterProxyModel doesn't support whole word matching directly,
            // we'll use a regex pattern for this case too
            searchText = QString("\\b%1\\b").arg(QRegularExpression::escape(searchText));
            m_proxyModel->setFilterRegExp(searchText);
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
