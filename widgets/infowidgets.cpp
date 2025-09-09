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
}

InfoWidgets::~InfoWidgets()
{
    delete ui;
}

void InfoWidgets::on_search_btn_clicked()
{
    if (ui->search_le->text().isEmpty()){
        if (ui->detail_tb->rowCount() < m_data_tb.size()){
            update_data_detail_tb(m_data_tb);
        }
        return;
    }

    QList<QStringList> match_tb;
    for (auto line : m_data_tb) {
        for (auto it : line) {
            if (it.contains(ui->search_le->text().trimmed(), Qt::CaseInsensitive)){
                match_tb.append(line);
                break;
            }
        }
    }

    update_data_detail_tb(match_tb);
}

void InfoWidgets::clear_detail_tb()
{
    ui->detail_tb->clear();
}

void InfoWidgets::init_header_detail_tb(const QStringList &headers)
{
    ui->detail_tb->setColumnCount(headers.size());

    ui->detail_tb->setHorizontalHeaderLabels(headers);

    ui->detail_tb->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    if (headers.size() > 0) {
        ui->detail_tb->horizontalHeader()->setSectionResizeMode(headers.size() - 1, QHeaderView::Stretch);
    }

    if (m_headers.isEmpty()) {
        m_headers = headers;
    }
}

void InfoWidgets::update_data_detail_tb(const QList<QStringList> &data_tb)
{
    ui->detail_tb->setRowCount(0);

    ui->detail_raw_pte->clear();
    for (int row = 0; row < data_tb.size(); ++row) {
        ui->detail_tb->insertRow(row);

        const auto& rowData = data_tb.at(row);

        for (int col = 0; col < rowData.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(rowData[col]);
            item->setTextAlignment(Qt::AlignCenter);
            ui->detail_tb->setItem(row, col, item);
        }

        ui->detail_raw_pte->appendPlainText(data_tb.at(row).join("="));
    }
    if (m_data_tb.isEmpty()) {
        m_data_tb = data_tb;
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


void InfoWidgets::on_header_btn_clicked()
{

}

