// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "infotablewg.h"
#include "qdebug.h"
#include "ui_infotablewg.h"
#include <QtGlobal>
#include <QPoint>

InfoWidgets::InfoWidgets(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoWidgets)
    , m_searchButtonMenu(nullptr)
    , m_detailSearchAction(nullptr)
    , m_copySelectedTextAction(nullptr)
    , m_copySelectedTextWithHeaderAction(nullptr)
    , m_detailSearchDialog(nullptr)
    , m_tableContextMenu(new QMenu(this))
    , m_isUserAdjusted(false)
    , m_resizeTimer(new QTimer(this))
    , m_lastTableWidth(0)
{
    ui->setupUi(this);
    ui->detail_raw_pte->setVisible(false);

    // Enable context menu for table
    ui->detail_tb->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->detail_tb, &QTableView::customContextMenuRequested, [this](const QPoint &pos) {
        m_detailAction->setVisible(HELP_OPTION_FORMATS.contains(m_helpKey));

        QModelIndex index = ui->detail_tb->indexAt(pos);
        if (index.isValid()) {
            m_currentRow = index.row();
            m_currentColumn = index.column();
            m_tableContextMenu->exec(ui->detail_tb->viewport()->mapToGlobal(pos));
        }
    });

    // detail info action
    m_detailAction = new QAction("Detail Info", this);
    connect(m_detailAction, &QAction::triggered, this, &InfoWidgets::showDetailInfo);
    m_tableContextMenu->addAction(m_detailAction);

    // restore order action
    m_restoreOrderAction = new QAction("Restore Order", this);
    connect(m_restoreOrderAction, &QAction::triggered, [=](){
        QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel*>(ui->detail_tb->model());
        if (!proxyModel) return;
        proxyModel->sort(-1, Qt::AscendingOrder);
        // multiColumnSearchModel->sort(-1, Qt::AscendingOrder); // both two could satisify
        ui->detail_tb->horizontalHeader()->setSortIndicator(-1, Qt::AscendingOrder);
        ui->detail_tb->horizontalHeader()->setSortIndicatorShown(false);
    });
    m_tableContextMenu->addAction(m_restoreOrderAction);

    m_tableContextMenu->addSeparator();

    // copy selected text action
    m_copySelectedTextAction = new QAction("Coyp Select Text", this);
    connect(m_copySelectedTextAction, &QAction::triggered, this, &InfoWidgets::copySelectedText);
    m_tableContextMenu->addAction(m_copySelectedTextAction);

    // copy selected text with header action
    m_copySelectedTextWithHeaderAction = new QAction("Coyp Select Text With Header", this);
    connect(m_copySelectedTextWithHeaderAction, &QAction::triggered, this, &InfoWidgets::copySelectedTextWithHeader);
    m_tableContextMenu->addAction(m_copySelectedTextWithHeaderAction);

    m_tableContextMenu->addSeparator();

    // copy selected text with header action
    m_fitTableColumnAction = new QAction("Fit Column Width", this);
    connect(m_fitTableColumnAction, &QAction::triggered, this, &InfoWidgets::fitTableColumnToContent);
    m_tableContextMenu->addAction(m_fitTableColumnAction);

    // model
    m_model = new MediaInfoTabelModel(this);

    multiColumnSearchModel = new MultiColumnSearchProxyModel(this);
    multiColumnSearchModel->setSourceModel(m_model);
    ui->detail_tb->setModel(multiColumnSearchModel);

    ui->detail_tb->horizontalHeader()->setSectionsMovable(true);
    ui->detail_tb->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->detail_tb->verticalHeader()->setDefaultSectionSize(25);
    ui->detail_tb->verticalHeader()->setVisible(true);
    ui->detail_tb->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->detail_tb->setSortingEnabled(true);

    // header manager
    m_headerManager = new ZTableHeaderManager(ui->detail_tb->horizontalHeader(), ui->detail_tb->verticalHeader(), this);
    m_headerManager->setObjectName(this->objectName());
    m_headerManager->setTotalCountVisible(false);
    m_headerManager->restoreState();

    connect(m_headerManager, &ZTableHeaderManager::headerToggleVisiable, [=]() {
        fitTableColumnToContent();
    });
    
    // Setup column width management
    setupColumnWidthManagement();
    
    // Setup search button with right-click menu
    setupSearchButton();

    new QShortcut(QKeySequence("Ctrl+F"), this, SLOT(showDetailSearch()));

    connect(m_model, &QAbstractItemModel::dataChanged, this,
            [=](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) {

        emit dataChanged(m_data_tb.at(topLeft.row()));
    });
}

InfoWidgets::~InfoWidgets()
{
    delete m_headerManager;
    delete m_detailSearchDialog;
    delete ui;
}

void InfoWidgets::setHelpInfoKey(const QString &key)
{
    m_helpKey = key;
}

void InfoWidgets::setSearchTitleVisiable(const bool &visiable)
{
    ui->search_title_wg->setVisible(visiable);
}

void InfoWidgets::addContextMenu(QMenu *menu)
{
    m_tableContextMenu->addMenu(menu);
}

void InfoWidgets::addContextAction(QAction *action)
{
    m_tableContextMenu->addAction(action);
}

void InfoWidgets::addContextSeparator()
{
    m_tableContextMenu->addSeparator();
}

QList<QStringList> InfoWidgets::getSelectLines()
{
    QList<QStringList> tmp;
    QSet<int> availableRow;
    QModelIndexList selectedRows = ui->detail_tb->selectionModel()->selectedIndexes();
    foreach (const QModelIndex &it, selectedRows) {
        auto index = multiColumnSearchModel->mapToSource(it);
        availableRow.insert(index.row());
    }

    foreach (auto &row, availableRow) {
        tmp.append(m_data_tb.at(row));
    }

    return tmp;
}

QList<int> InfoWidgets::getSelectRows()
{
    QList<int> tmp;
    QSet<int> availableRow;
    QModelIndexList selectedRows = ui->detail_tb->selectionModel()->selectedIndexes();
    foreach (const QModelIndex &it, selectedRows) {
        auto index = multiColumnSearchModel->mapToSource(it);
        availableRow.insert(index.row());
    }

    foreach (auto &row, availableRow) {
        tmp.append(row);
    }

    return tmp;
}

const QList<QStringList> *InfoWidgets::getTableData()
{
    return &m_data_tb;
}

void InfoWidgets::on_search_btn_clicked()
{
    onDetailSearchCompleted();
}

void InfoWidgets::clear_detail_tb()
{
    m_model->setRow(0);
}

void InfoWidgets::init_header_detail_tb(const QStringList &headers, QString format_join)
{
    m_headers = headers;
    
    m_model->setColumn(m_headers.count());
    m_model->setTableHeader(&m_headers);

    // Initial resize mode will be set in setupInitialColumnWidths()
    m_headerManager->restoreState();

    ui->detail_raw_pte->clear();
    ui->detail_raw_pte->appendPlainText(m_headers.join(format_join));
}

void InfoWidgets::update_data_detail_tb(const QList<QStringList> &data_tb, QString format_join)
{
    m_data_tb = data_tb;

    m_model->setRow(m_data_tb.count());
    m_model->setTableData(const_cast<QList<QStringList>*>(&m_data_tb));

    if (!ui->detail_tb->model()) {
        ui->detail_tb->setModel(multiColumnSearchModel);
    }
    
    ui->detail_tb->setShowGrid(true);

    updateCurrentModel();

    if (m_headers.size() > 0) {
        setupInitialColumnWidths();
    }
    ui->detail_raw_pte->clear();
    for (auto it : m_data_tb) {
        ui->detail_raw_pte->appendPlainText(it.join(format_join));
    }

    QTimer::singleShot(50, this, [this]() {
        resizeColumnsProportionally();
    });
}

void InfoWidgets::update_data_detail_tb(const QMap<QString, QList<QStringList> > &data_tb, QString format_join)
{
    m_data_tb.clear();
    for (auto key: data_tb.keys()) {
        m_data_tb.append(data_tb.value(key));
    }

    m_model->setRow(m_data_tb.count());
    m_model->setTableData(const_cast<QList<QStringList>*>(&m_data_tb));

    if (!ui->detail_tb->model()) {
        ui->detail_tb->setModel(multiColumnSearchModel);
    }

    ui->detail_tb->setShowGrid(true);

    updateCurrentModel();

    if (m_headers.size() > 0) {
        setupInitialColumnWidths();
    }
    ui->detail_raw_pte->clear();
    for (auto it : m_data_tb) {
        ui->detail_raw_pte->appendPlainText(it.join(format_join));
    }
}

void InfoWidgets::remove_data_from_row_indexs(const QList<int> &indexs)
{
    QList<int> sortedRows = indexs;
    std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());
    foreach (int row, sortedRows) {
        m_data_tb.removeAt(row);
    }
}

void InfoWidgets::append_data_detail_tb(const QList<QStringList> &data_tb, QString format_join)
{
    m_data_tb.append(data_tb);

    m_model->setRow(m_data_tb.count());
    m_model->setTableData(const_cast<QList<QStringList>*>(&m_data_tb));

    if (!ui->detail_tb->model()) {
        ui->detail_tb->setModel(multiColumnSearchModel);
    }

    ui->detail_tb->setShowGrid(true);

    updateCurrentModel();

    if (m_headers.size() > 0) {
        setupInitialColumnWidths();
    }
    ui->detail_raw_pte->clear();
    for (auto it : m_data_tb) {
        ui->detail_raw_pte->appendPlainText(it.join(format_join));
    }
}

void InfoWidgets::remove_selected_row()
{
    QSet<int> availableRow;
    QModelIndexList selectedRows = ui->detail_tb->selectionModel()->selectedIndexes();
    foreach (const QModelIndex &index, selectedRows) {
        availableRow.insert(index.row());
    }

    QList<int> rowsToDelete = availableRow.values();
    std::sort(rowsToDelete.begin(), rowsToDelete.end(), std::greater<int>());

    foreach (int row, rowsToDelete) {
        if (row >= 0 && row < m_data_tb.size()) {
            m_data_tb.removeAt(row);
        }
    }

    m_model->setRow(m_data_tb.count());
}

void InfoWidgets::format_data(const QString &data, QList<QStringList> &data_tb, QStringList &headers, QString format_key)
{
    headers.clear();
    data_tb.clear();

    QStringList rawStringLines = data.split("\n", QT_SKIP_EMPTY_PARTS);

    if (rawStringLines.size() < 1)
        return;

    if (rawStringLines.at(0).contains(":"))
        rawStringLines.removeFirst();

    // -L -sources -sinks
    if (QStringList{"L", "sources", "sinks"}.contains(format_key, Qt::CaseInsensitive)){
        headers << tr("Info");
        for (auto it : rawStringLines) {
            data_tb << QStringList{it};
        }
        return;
    }

    // -version
    if (QStringList{"version"}.contains(format_key, Qt::CaseInsensitive)) {
        headers << tr("Config") << "Value";

        data_tb.append(QStringList{tr("Version"), rawStringLines.at(0)});
        data_tb.append(QStringList{tr("Build"), rawStringLines.at(1)});

        for (int i = 2; i < rawStringLines.size(); ++i) {
            QStringList tmp = rawStringLines.at(i).split(" ", QT_SKIP_EMPTY_PARTS);
            if (tmp.size() > 0 && tmp.at(0).contains("configuration"))
                tmp.takeFirst();

            QStringList tb_of_line;
            if (rawStringLines.at(i).contains("=")){
                for (auto it : tmp) {
                    QStringList keyParts = it.split("=", QT_SKIP_EMPTY_PARTS);
                    tb_of_line.clear();
                    if (keyParts.size() == 2) {
                        tb_of_line << keyParts[0].replace("--", "").trimmed() << keyParts.at(1).trimmed();
                    } else {
                        tb_of_line << "" << keyParts[0].replace("--", "").trimmed();
                    }
                    data_tb.append(tb_of_line);
                }
            } else {
                tb_of_line.append(tmp.at(0).trimmed());
                tmp.removeFirst();
                tb_of_line.append(tmp.join(" "));
                data_tb.append(tb_of_line);
            }
        }
        return;
    }

    // -formats -muxers -demuxers -devices -codecs -decoders -encoders -filters -pix_fmts
    if (QStringList{"formats", "muxers", "demuxers", "devices", "codecs", "decoders", "filters", "encoders", "pixfmts"}.contains(format_key, Qt::CaseInsensitive)) {
        QStringList parts{QString(""), QString("")};
        if (format_key.contains("filters")) {
            for (auto it : rawStringLines) {
                if (it.contains("=")) {
                    parts[0].append(it + "\n");
                } else {
                    parts[1].append(it + "\n");
                }
            }
        } else {
            parts = data.split("--", QT_SKIP_EMPTY_PARTS);
        }

        if (parts.size() >= 2) {

            // header
            QString head_code = "";
            auto tmp_headers = parts.at(0).split("\n", QT_SKIP_EMPTY_PARTS);
            for (auto it : tmp_headers) {
                if (it.contains(":"))
                    continue;

                if (it.contains("=")) {
                    QStringList tmp = it.split("=", QT_SKIP_EMPTY_PARTS);
                    if (tmp.size() >= 2) {
                        headers.append("[" +tmp[0].replace(".", "").trimmed() + "]" + tmp.at(1).trimmed());
                        head_code.append(tmp[0].replace(".", "").trimmed());
                    }
                }
            }
            headers.append(tr("name"));
            if (format_key.contains("pixfmts")) {
                headers.append(tr("NB_COMPONENTS"));
                headers.append(tr("BITS_PER_PIXEL"));
            } else if (format_key.contains("filters")){
                headers.append(tr("direct"));
            } else {
                headers.append(tr("detail"));
            }

            qDebug() << "header size:" << headers.size() << "head_code: " << head_code;

            // content
            auto tmp_content = parts.at(1).split("\n", QT_SKIP_EMPTY_PARTS);
            for (auto it : tmp_content) {
                QStringList tb_of_line;

                QStringList tmp = it.split(" ", QT_SKIP_EMPTY_PARTS);
                if (tmp.size() < 3){
                    continue;
                }
                for (int ch = 0; ch < head_code.size(); ++ch) {
                    tb_of_line.append("");
                }
                for (auto ch : tmp.at(0).trimmed()) {
                    int index = head_code.lastIndexOf(ch);

                    if (index >= 0) {
                        tb_of_line[index] = "√";
                    }
                }

                tb_of_line.append(tmp.at(1).trimmed());

                tmp.removeFirst();
                tmp.removeFirst();

                if (format_key.contains("pixfmts")) {
                    tb_of_line.append(tmp.at(0).trimmed());
                    tb_of_line.append(tmp.at(1).trimmed());
                }else {
                    tb_of_line.append(tmp.join(" "));
                }

                data_tb.append(tb_of_line);
            }
        }

        return;
    }

    // -colors -sample_fmts -layouts (inner: videorate, videosize)
    if (QStringList{"colors", "samplefmts", "layouts", "videorate", "videosize"}.contains(format_key, Qt::CaseInsensitive)) {
        headers << tr("Name") << "Value";

        for (int i = 1; i < rawStringLines.size(); ++i) {
            if (rawStringLines.at(i).contains(":", Qt::CaseInsensitive) ||
                rawStringLines.at(i).contains("DECOMPOSITION", Qt::CaseInsensitive) ||
                rawStringLines.at(i).contains("DESCRIPTION", Qt::CaseInsensitive)) {
                continue;
            }
            QStringList tmp = rawStringLines.at(i).split(" ", QT_SKIP_EMPTY_PARTS);
            if (tmp.size() >= 2) {
                QStringList tb_of_line{tmp.at(0).trimmed()};
                tmp.removeFirst();
                tb_of_line.append(tmp.join(" "));
                data_tb.append(tb_of_line);
            }
        }

        return;
    }

    // -protocols
    if (QStringList{"protocols"}.contains(format_key, Qt::CaseInsensitive)) {
        headers << tr("Input") << "Output";

        QStringList input, output;
        bool startOutput = false;
        for (int i = 2; i < rawStringLines.size(); i++) {
            if (rawStringLines.at(i).contains("output", Qt::CaseInsensitive)){
                startOutput = true;
                continue;
            }
            if (startOutput){
                output.append(rawStringLines.at(i).trimmed());
            } else {
                input.append(rawStringLines.at(i).trimmed());
            }
        }

        int maxRow = std::max(input.size(), output.size());

        for (int i = 0; i < maxRow; i++) {
            QStringList tmpRow;
            if (input.size() <= i) {
                tmpRow.append("");
            } else {
                tmpRow.append(input.at(i));
            }

            if (output.size() <= i) {
                tmpRow.append("");
            } else {
                tmpRow.append(output.at(i));
            }

            data_tb.append(tmpRow);
        }

        return;
    }

    // -bsfs -buildconf
    if (QStringList{"bsfs", "buildconf"}.contains(format_key, Qt::CaseInsensitive)) {
        headers.append(tr("name"));

        for (int i = 0; i < rawStringLines.size(); i++) {
            data_tb.append(QStringList{rawStringLines.at(i).trimmed()});
        }

        return;
    }

    if (format_key == "") {

    }
}

void InfoWidgets::init_detail_tb(const QString &data, const QString& format_key)
{
    format_data(data, m_data_tb, m_headers, format_key);

    init_header_detail_tb(m_headers);
    update_data_detail_tb(m_data_tb);
}

void InfoWidgets::on_expand_raw_btn_clicked(bool checked)
{
    ui->detail_raw_pte->setVisible(checked);
}

void InfoWidgets::on_search_le_editingFinished()
{
    emit ui->search_btn->clicked();
}

void InfoWidgets::setupSearchButton()
{
    // Create right-click menu for search button
    m_searchButtonMenu = new QMenu(this);
    m_detailSearchAction = new QAction(tr("Detail Search"), this);
    m_detailSearchAction->setToolTip(tr("Open advanced search dialog with more options"));
    
    m_searchButtonMenu->addAction(m_detailSearchAction);
    
    // Connect the action
    createDetailSearchDialog();
    connect(m_detailSearchAction, &QAction::triggered, this, &InfoWidgets::showDetailSearch);
    
    // Set context menu policy for search button
    ui->search_btn->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->search_btn, &QPushButton::customContextMenuRequested, 
            this, [this](const QPoint &pos) {
                m_searchButtonMenu->exec(ui->search_btn->mapToGlobal(pos));
            });
}

void InfoWidgets::createDetailSearchDialog()
{
    if (!m_detailSearchDialog) {
        m_detailSearchDialog = new SearchWG(this);
        m_detailSearchDialog->setWindowTitle(tr("Detail Search"));
        m_detailSearchDialog->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
        // m_detailSearchDialog->setWindowModality(Qt::ApplicationModal);
        
        // Configure to show only the required group boxes for InfoWidgets
        auto requiredBoxes = SearchWG::SearchRange | SearchWG::MatchControl | SearchWG::Operation;
        m_detailSearchDialog->setVisibleGroupBoxes(requiredBoxes);
        
        // Set search range options based on current table headers
        if (!m_headers.isEmpty()) {
            m_detailSearchDialog->setSearchRangeOptions(m_headers);
        }
        
        // Connect signals
        connect(m_detailSearchDialog, &SearchWG::searchRangeSelectionChanged,
                this, &InfoWidgets::onDetailSearchCompleted);

        connect(m_detailSearchDialog, &SearchWG::searchReady,
                this, &InfoWidgets::onDetailSearchCompleted);

        connect(m_detailSearchDialog, &SearchWG::searchTextChanged, [=](QString text) {
            ui->search_le->setText(text);
        });

        // Resize dialog appropriately
        m_detailSearchDialog->adjustSize();
        m_detailSearchDialog->setMinimumWidth(400);
    }
}

void InfoWidgets::showDetailSearch()
{
    // Show search title
    ui->search_title_wg->setVisible(!ui->search_title_wg->isVisible());

    // Update search range options with current headers
    if (!m_headers.isEmpty()) {
        m_detailSearchDialog->setSearchRangeOptions(m_headers);
    }
    
    // Show the dialog
    m_detailSearchDialog->show();
    m_detailSearchDialog->raise();
    m_detailSearchDialog->activateWindow();
}

void InfoWidgets::onDetailSearchCompleted()
{
    // Get search parameters from SearchWG
    QStringList selectedRanges = m_detailSearchDialog->getSelectedSearchRanges();
    QString searchText = ui->search_le->text().trimmed();
    
    qDebug() << "Detail search completed with selected ranges:" << selectedRanges;
    qDebug() << "Search text:" << searchText;
    
    if (searchText.isEmpty()) {
        ui->detail_tb->setModel(multiColumnSearchModel);
        multiColumnSearchModel->resetFilters();
        updateCurrentModel();
        return;
    }
    
    // Configure multi-column search model
    multiColumnSearchModel->setSearchText(searchText);
    
    // Set match control options from SearchWG
    multiColumnSearchModel->setCaseSensitive(m_detailSearchDialog->isCaseSensitive());
    multiColumnSearchModel->setMatchWholeWords(m_detailSearchDialog->isMatchWholewords());
    multiColumnSearchModel->setUseRegularExpression(m_detailSearchDialog->isUseRegularExpression());
    
    // Configure search columns
    if (selectedRanges.isEmpty()) {
        // No columns selected, search in all columns
        multiColumnSearchModel->setSearchMode(false);
        qDebug() << "Searching in all columns";
    } else {
        // Search only in selected columns
        multiColumnSearchModel->setSearchColumns(selectedRanges);
        multiColumnSearchModel->setSearchMode(true);
        qDebug() << "Searching in selected columns:" << selectedRanges;
    }

    ui->detail_tb->setModel(multiColumnSearchModel);
    
    updateCurrentModel();
    
    qDebug() << "Multi-column search applied. Filtered rows:" << multiColumnSearchModel->rowCount();
}

void InfoWidgets::updateCurrentModel()
{
    m_headerManager->updateTotalCount(multiColumnSearchModel->rowCount());
}

QString InfoWidgets::getSelectedText(bool includeHeader)
{
    QModelIndexList selected = ui->detail_tb->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return QString();
    }

    QString text;
    QItemSelectionRange range = ui->detail_tb->selectionModel()->selection().first();

    if (includeHeader) {
        QStringList headerContents;
        for (int j = range.left(); j <= range.right(); ++j) {
            headerContents << ui->detail_tb->model()->headerData(j, Qt::Horizontal).toString();
        }
        text += headerContents.join("\t") + "\n";
    }

    for (int i = range.top(); i <= range.bottom(); ++i) {
        QStringList rowContents;
        for (int j = range.left(); j <= range.right(); ++j) {
            rowContents << ui->detail_tb->model()->index(i,j).data().toString();
        }
        text += rowContents.join("\t") + "\n";
    }

    return text;
}

void InfoWidgets::on_search_le_textChanged(const QString &arg1)
{
    if (m_detailSearchDialog) {
        m_detailSearchDialog->setSearchText(arg1);
    }
}

void InfoWidgets::copySelectedText()
{
    QString text = getSelectedText(false);
    if (!text.isEmpty()) {
        QApplication::clipboard()->setText(text);
    }
}

void InfoWidgets::copySelectedTextWithHeader()
{
    QString text = getSelectedText(true);
    if (!text.isEmpty()) {
        QApplication::clipboard()->setText(text);
    }
}

void InfoWidgets::fitTableColumnToContent()
{
    QTimer::singleShot(50, this, [this]() {
        resizeColumnsProportionally();
    });
}

void InfoWidgets::showDetailInfo()
{
    QModelIndex currentIndex = ui->detail_tb->currentIndex();

    int columnIndex = m_headers.indexOf("name");
    int rowIndex = -1;
    auto index = multiColumnSearchModel->mapToSource(currentIndex);

    if (m_helpKey == PROTOCOL_FMT) {
        columnIndex = index.column();
    }
    rowIndex = index.row();

    if (columnIndex < 0 || rowIndex < 0) {
        qCritical() << "can't find selected column or row" << columnIndex << rowIndex;
        return;
    }

    qDebug() << "current name-s: " << m_data_tb[rowIndex][columnIndex];

    HelpQueryWg *helpWindow = new HelpQueryWg;
    helpWindow->setAttribute(Qt::WA_DeleteOnClose);
    helpWindow->setControlHeaderVisiable(false);

    helpWindow->setWindowTitle(tr("Help Query %1=%2").arg(m_helpKey).arg(m_data_tb[rowIndex][columnIndex]));
    helpWindow->setHelpParams(m_helpKey, m_data_tb[rowIndex][columnIndex]);
    helpWindow->show();
    ZWindowHelper::centerToParent(helpWindow);
}

void InfoWidgets::setupColumnWidthManagement()
{
    // Setup resize timer for performance optimization
    // m_resizeTimer->setSingleShot(true);
    m_resizeTimer->setInterval(100); // 100ms delay
    connect(m_resizeTimer, &QTimer::timeout, this, &InfoWidgets::onResizeTimerTimeout);
    
    // Connect header resize signal
    connect(ui->detail_tb->horizontalHeader(), &QHeaderView::sectionResized,
            this, &InfoWidgets::onHeaderSectionResized);
    
    // Install event filter on table view to catch resize events
    ui->detail_tb->installEventFilter(this);
}

void InfoWidgets::saveColumnWidthRatios()
{
    if (m_headers.isEmpty() || !ui->detail_tb->horizontalHeader()) {
        return;
    }
    
    QHeaderView *header = ui->detail_tb->horizontalHeader();
    int totalWidth = 0;
    m_columnWidthRatios.clear();
    
    // Calculate total width
    for (int i = 0; i < m_headers.size(); ++i) {
        totalWidth += header->sectionSize(i);
    }
    
    if (totalWidth <= 0) {
        return;
    }
    
    // Calculate ratios
    for (int i = 0; i < m_headers.size(); ++i) {
        double ratio = static_cast<double>(header->sectionSize(i)) / totalWidth;
        m_columnWidthRatios.append(ratio);
    }
    
    m_lastTableWidth = ui->detail_tb->viewport()->width();
    // qDebug() << "Saved column width ratios:" << m_columnWidthRatios;
}

void InfoWidgets::restoreColumnWidthRatios()
{
    if (m_columnWidthRatios.isEmpty() || m_headers.isEmpty() || !ui->detail_tb->horizontalHeader()) {
        return;
    }
    
    QHeaderView *header = ui->detail_tb->horizontalHeader();
    int availableWidth = ui->detail_tb->viewport()->width();

    if (availableWidth <= 0) {
        return;
    }
    
    // Temporarily disable stretch mode
    header->setSectionResizeMode(QHeaderView::Interactive);
    
    // Apply ratios
    for (int i = 0; i < qMin(m_columnWidthRatios.size(), m_headers.size()); ++i) {
        int newWidth = static_cast<int>(availableWidth * m_columnWidthRatios[i]);
        newWidth = qMax(newWidth, 50); // Minimum width
        header->resizeSection(i, newWidth);
    }
    
    m_lastTableWidth = availableWidth;
}

void InfoWidgets::resizeColumnsProportionally()
{
    if (!m_isUserAdjusted || m_columnWidthRatios.isEmpty() || m_headers.isEmpty()) {
        setupInitialColumnWidths();
        return;
    }

    restoreColumnWidthRatios();
}

void InfoWidgets::setupInitialColumnWidths()
{
    if (m_headers.isEmpty() || !ui->detail_tb->horizontalHeader()) {
        return;
    }
    
    QHeaderView *header = ui->detail_tb->horizontalHeader();
    
    // If user hasn't adjusted columns, use automatic sizing
    if (!m_isUserAdjusted) {
        header->setSectionResizeMode(QHeaderView::ResizeToContents);
        
        // Process events to ensure content-based sizing is applied
        QApplication::processEvents();
        
        // Set last column to stretch if there are multiple columns
        if (m_headers.size() > 1) {
            header->setSectionResizeMode(m_headers.size() - 1, QHeaderView::Stretch);
        }
        
        // Save initial ratios after auto-sizing
        QTimer::singleShot(50, this, [this]() {
            saveColumnWidthRatios();
        });
    } else {
        // Restore user-adjusted proportions
        restoreColumnWidthRatios();
    }
}

void InfoWidgets::onHeaderSectionResized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(logicalIndex)
    Q_UNUSED(oldSize)
    Q_UNUSED(newSize)
    
    // Mark as user-adjusted when user manually resizes columns
    if (!m_isUserAdjusted) {
        m_isUserAdjusted = true;
        qDebug() << "Column widths marked as user-adjusted";
    }
    
    // Use timer to avoid frequent updates during dragging
    m_resizeTimer->start();
}

void InfoWidgets::onResizeTimerTimeout()
{
    // Save new ratios after user adjustment
    saveColumnWidthRatios();
}

bool InfoWidgets::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->detail_tb && event->type() == QEvent::Resize) {
        QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
        int newWidth = resizeEvent->size().width();
        
        // Only trigger proportional resize if width changed significantly
        if (qAbs(newWidth - m_lastTableWidth) > 10) {
            // Use timer to avoid frequent resizing during window drag
            QTimer::singleShot(50, this, [this]() {
                resizeColumnsProportionally();
            });
        }
    }
    
    return QWidget::eventFilter(obj, event);
}

