#include "infowidgets.h"
#include "qdebug.h"
#include "ui_infowidgets.h"
#include <QtGlobal>

InfoWidgets::InfoWidgets(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::InfoWidgets)
{
    ui->setupUi(this);
    ui->detail_raw_pte->setVisible(false);

    model = new MediaInfoTabelModel;

    ui->detail_tb->horizontalHeader()->setSectionsMovable(true);
    ui->detail_tb->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->detail_tb->horizontalHeader(), &QHeaderView::customContextMenuRequested,
            this, &InfoWidgets::onHeaderContextMenuRequested);
}

InfoWidgets::~InfoWidgets()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.setValue(this->objectName(), ui->detail_tb->horizontalHeader()->saveState());
    settings.sync();
    delete ui;
}

void InfoWidgets::on_search_btn_clicked()
{
    if (ui->search_le->text().isEmpty()){
        if (m_match_tb.size() < m_data_tb.size()){
            update_data_detail_tb(m_data_tb);
        }
        return;
    }

    m_match_tb.clear();
    for (auto line : m_data_tb) {
        for (auto it : line) {
            if (it.contains(ui->search_le->text().trimmed(), Qt::CaseInsensitive)){
                m_match_tb.append(line);
                break;
            }
        }
    }

    update_data_detail_tb(m_match_tb);
}

void InfoWidgets::clear_detail_tb()
{
    model->setRow(0);
}

void InfoWidgets::init_header_detail_tb(const QStringList &headers, QString format_join)
{
    model->setColumn(headers.count());
    model->setTableHeader(&m_headers);

    ui->detail_tb->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    if (m_headers.isEmpty()) {
        m_headers = headers;
    }

    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    if (settings.contains(this->objectName())) {
        QByteArray columnOrder = settings.value(this->objectName()).toByteArray();
        ui->detail_tb->horizontalHeader()->restoreState(columnOrder);
    }

    ui->detail_raw_pte->clear();
    ui->detail_raw_pte->appendPlainText(m_headers.join(format_join));
}

void InfoWidgets::update_data_detail_tb(const QList<QStringList> &data_tb, QString format_join)
{
    model->setRow(data_tb.count());
    if (m_data_tb.isEmpty()) {
        m_match_tb = data_tb;
    }

    if (m_data_tb.size() >= data_tb.size()) {
        model->setTableData(const_cast<QList<QStringList>*>(&data_tb));
    } else {
        m_data_tb = data_tb;
        model->setTableData(const_cast<QList<QStringList>*>(&m_data_tb));
    }

    ui->detail_tb->setModel(model);
    ui->detail_tb->setShowGrid(true);

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
    QIcon icon;
    if (checked) {
        icon = (QIcon::fromTheme(QString::fromUtf8("QIcon::ThemeIcon::GoNext")));
    } else {
        icon = (QIcon::fromTheme(QString::fromUtf8("QIcon::ThemeIcon::GoDown")));
    }
    ui->expand_raw_btn->setIcon(icon);
    ui->detail_raw_pte->setVisible(checked);
}

void InfoWidgets::on_search_le_editingFinished()
{
    emit ui->search_btn->clicked();
}

void InfoWidgets::onHeaderContextMenuRequested(const QPoint &pos)
{
    QHeaderView *header = ui->detail_tb->horizontalHeader();
    QAbstractItemModel *model = ui->detail_tb->model();

    if (!model) return;

    QMenu menu(this);
    m_actionToColumnMap.clear();

    QAction *showAllAction = menu.addAction("Show All");
    QAction *hideAllAction = menu.addAction("Hide All");
    menu.addSeparator();

    connect(showAllAction, &QAction::triggered, this, &InfoWidgets::showAllColumns);
    connect(hideAllAction, &QAction::triggered, this, &InfoWidgets::hideAllColumns);

    for (int logicalIndex = 0; logicalIndex < header->count(); ++logicalIndex) {
        QString columnName = model->headerData(logicalIndex, Qt::Horizontal).toString();
        if (columnName.isEmpty()) {
            columnName = QString("Column %1").arg(logicalIndex + 1);
        }

        QAction *columnAction = menu.addAction(columnName);
        columnAction->setCheckable(true);
        columnAction->setChecked(!header->isSectionHidden(logicalIndex));

        m_actionToColumnMap[columnAction] = logicalIndex;

        connect(columnAction, &QAction::triggered, this, &InfoWidgets::toggleColumnVisibility);
    }

    menu.exec(header->mapToGlobal(pos));
}

void InfoWidgets::toggleColumnVisibility()
{
    QAction *action = qobject_cast<QAction*>(sender());
    if (!action || !m_actionToColumnMap.contains(action)) return;

    int logicalIndex = m_actionToColumnMap[action];
    QHeaderView *header = ui->detail_tb->horizontalHeader();

    if (action->isChecked()) {
        header->showSection(logicalIndex);
    } else {
        header->hideSection(logicalIndex);
    }
}

void InfoWidgets::showAllColumns()
{
    QHeaderView *header = ui->detail_tb->horizontalHeader();
    for (int i = 0; i < header->count(); ++i) {
        header->showSection(i);
    }
}

void InfoWidgets::hideAllColumns()
{
    QHeaderView *header = ui->detail_tb->horizontalHeader();
    for (int i = 0; i < header->count(); ++i) {
        header->hideSection(i);
    }
}
