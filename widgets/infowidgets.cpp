#include "infowidgets.h"
#include "qdebug.h"
#include "ui_infowidgets.h"
#include <QtGlobal>
#include <QPoint>

InfoWidgets::InfoWidgets(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoWidgets)
    , m_searchButtonMenu(nullptr)
    , m_detailSearchAction(nullptr)
    , m_detailSearchDialog(nullptr)
    , m_tableContextMenu(new QMenu(this))
{
    ui->setupUi(this);
    ui->detail_raw_pte->setVisible(false);

    // Enable context menu for table
    ui->detail_tb->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->detail_tb, &QTableView::customContextMenuRequested, [this](const QPoint &pos) {
        QModelIndex index = ui->detail_tb->indexAt(pos);
        if (index.isValid()) {
            currentRow = index.row();
            currentColumn = index.column();
            m_tableContextMenu->exec(ui->detail_tb->viewport()->mapToGlobal(pos));
        }
    });
    QAction *detailAction = new QAction("Detail Info", this);
    connect(detailAction, &QAction::triggered, this, &InfoWidgets::showDetailInfo);
    m_tableContextMenu->addAction(detailAction);

    // model
    model = new MediaInfoTabelModel(this);

    multiColumnSearchModel = new MultiColumnSearchProxyModel(this);
    multiColumnSearchModel->setSourceModel(model);
    ui->detail_tb->setModel(multiColumnSearchModel);

    ui->detail_tb->horizontalHeader()->setSectionsMovable(true);
    ui->detail_tb->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->detail_tb->verticalHeader()->setDefaultSectionSize(25);
    ui->detail_tb->verticalHeader()->setVisible(true);
    ui->detail_tb->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->detail_tb->setSortingEnabled(true);

    m_headerManager = new TableHeaderManager(ui->detail_tb->horizontalHeader(), ui->detail_tb->verticalHeader(), this);
    m_headerManager->setObjectName(this->objectName());
    m_headerManager->setTotalCountVisible(false);
    m_headerManager->restoreState();
    
    // Setup search button with right-click menu
    setupSearchButton();

    new QShortcut(QKeySequence("Ctrl+F"), this, SLOT(showDetailSearch()));

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

void InfoWidgets::on_search_btn_clicked()
{
    onDetailSearchCompleted();
}

void InfoWidgets::clear_detail_tb()
{
    model->setRow(0);
}

void InfoWidgets::init_header_detail_tb(const QStringList &headers, QString format_join)
{
    m_headers = headers;
    
    model->setColumn(headers.count());
    model->setTableHeader(&m_headers);

    ui->detail_tb->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_headerManager->restoreState();

    ui->detail_raw_pte->clear();
    ui->detail_raw_pte->appendPlainText(m_headers.join(format_join));
}

void InfoWidgets::update_data_detail_tb(const QList<QStringList> &data_tb, QString format_join)
{
    model->setRow(data_tb.count());
    m_data_tb = data_tb;
    model->setTableData(const_cast<QList<QStringList>*>(&m_data_tb));

    // Don't change the model here - keep whatever model is currently active
    // Only set the model if no model is currently set
    if (!ui->detail_tb->model()) {
        ui->detail_tb->setModel(multiColumnSearchModel);
    }
    
    ui->detail_tb->setShowGrid(true);
    
    // Update using the helper method
    updateCurrentModel();

    if (m_headers.size() > 0 && ui->detail_tb->horizontalHeader()) {
        ui->detail_tb->horizontalHeader()->setSectionResizeMode(m_headers.size() - 1, QHeaderView::Stretch);
    }
    ui->detail_raw_pte->clear();
    for (auto it : data_tb) {
        ui->detail_raw_pte->appendPlainText(it.join(format_join));
    }
}

void InfoWidgets::format_data(const QString &data, QList<QStringList> &data_tb, QStringList &headers, QString format_key)
{
    headers.clear();
    data_tb.clear();

    QStringList rawStringLines = data.split("\n", QString::SkipEmptyParts);

    if (rawStringLines.size() < 1)
        return;

    if (rawStringLines.at(0).contains(":"))
        rawStringLines.removeFirst();

    // -L
    if (QStringList{"L"}.contains(format_key, Qt::CaseInsensitive)){
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
            QStringList tmp = rawStringLines.at(i).split(" ", QString::SkipEmptyParts);
            if (tmp.size() > 0 && tmp.at(0).contains("configuration"))
                tmp.takeFirst();

            QStringList tb_of_line;
            if (rawStringLines.at(i).contains("=")){
                for (auto it : tmp) {
                    QStringList keyParts = it.split("=", QString::SkipEmptyParts);
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
            parts = data.split("--", QString::SkipEmptyParts);
        }

        if (parts.size() >= 2) {

            // header
            QString head_code = "";
            auto tmp_headers = parts.at(0).split("\n", QString::SkipEmptyParts);
            for (auto it : tmp_headers) {
                if (it.contains(":"))
                    continue;

                if (it.contains("=")) {
                    QStringList tmp = it.split("=", QString::SkipEmptyParts);
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
            auto tmp_content = parts.at(1).split("\n", QString::SkipEmptyParts);
            for (auto it : tmp_content) {
                QStringList tb_of_line;

                QStringList tmp = it.split(" ", QString::SkipEmptyParts);
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

    // -colors -sample_fmts -layouts
    if (QStringList{"colors", "samplefmts", "layouts"}.contains(format_key, Qt::CaseInsensitive)) {
        headers << tr("Name") << "Value";

        for (int i = 1; i < rawStringLines.size(); ++i) {
            if (rawStringLines.at(i).contains(":", Qt::CaseInsensitive) ||
                rawStringLines.at(i).contains("DECOMPOSITION", Qt::CaseInsensitive) ||
                rawStringLines.at(i).contains("DESCRIPTION", Qt::CaseInsensitive)) {
                continue;
            }
            QStringList tmp = rawStringLines.at(i).split(" ", QString::SkipEmptyParts);
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
        headers << tr("Bitstream filters");

        for (int i = 1; i < rawStringLines.size(); i++) {
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
    // if (!m_detailSearchDialog) return;
    
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


void InfoWidgets::on_search_le_textChanged(const QString &arg1)
{
    if (m_detailSearchDialog) {
        m_detailSearchDialog->setSearchText(arg1);
    }
}

void InfoWidgets::copySelectedText()
{
    QModelIndexList selected = ui->detail_tb->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    QString text;
    QItemSelectionRange range = ui->detail_tb->selectionModel()->selection().first();
    for (int i = range.top(); i <= range.bottom(); ++i) {
        QStringList rowContents;
        for (int j = range.left(); j <= range.right(); ++j) {
            rowContents << ui->detail_tb->model()->index(i,j).data().toString();
        }
        text += rowContents.join("\t");
        text += "\n";
    }

    QApplication::clipboard()->setText(text);
}

void InfoWidgets::showDetailInfo()
{
    QModelIndex currentIndex = ui->detail_tb->currentIndex();

    int nameIndex = m_headers.indexOf("name");

    auto index = multiColumnSearchModel->mapToSource(currentIndex);

    qDebug() << "current name-s: " << m_data_tb[index.row()][nameIndex];

    HelpQueryWg *helpWindow = new HelpQueryWg;
    helpWindow->setAttribute(Qt::WA_DeleteOnClose);

    helpWindow->setWindowTitle("Help Query");
    helpWindow->setHelpParams(m_helpKey, m_data_tb[index.row()][nameIndex]);
    helpWindow->show();
    ZWindowHelper::centerToParent(helpWindow);
}

void InfoWidgets::exportSelectedData()
{
    QModelIndexList selected = ui->detail_tb->selectionModel()->selectedIndexes();
    if (selected.isEmpty()) {
        return;
    }

    // TODO: Implement actual export functionality
    qDebug() << "Exporting selected data";
}

