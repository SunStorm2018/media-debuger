#include "helpquerywg.h"
#include "ui_helpquerywg.h"

HelpQueryWg::HelpQueryWg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpQueryWg)
{
    ui->setupUi(this);
    ui->category_combx->addItems(HELP_OPTION_FORMATS);

    // qDebug() << "get codecs-encoder:" << m_probe.getCodecsFromLibav(CODEC_TYPE_ENCODER);
    // qDebug() << "get codecs-decoder:" << m_probe.getCodecsFromLibav(CODEC_TYPE_DECODER);
    // qDebug() << "get muxer-demuxer:" << m_probe.getMuxersFromLibav(MUXER_TYPE_DEMUXER);
    // qDebug() << "get muxer-muxer:" << m_probe.getMuxersFromLibav(MUXER_TYPE_MUXER);
    // qDebug() << "get filters:" << m_probe.getFiltersFromLibav();
    // qDebug() << "get bsf:" << m_probe.getBsfFromLibav();
    // qDebug() << "get protocols:" << m_probe.getProtocolFromLibav();

    emit ui->category_combx->activated(0);
}

HelpQueryWg::~HelpQueryWg()
{
    delete ui;
}

bool HelpQueryWg::setHelpParams(const QString &category, const QString &value)
{
    ui->search_output_ple->clear();

    QStringList helpList {
        QString("%1%2%3")
        .arg(category)
            .arg("=")
            .arg(value)
    };

    ui->search_output_ple->setPlainText(m_probe.getHelp(helpList));
}

void HelpQueryWg::on_search_btn_clicked()
{
    if (!ui->keep_last_cbx->isChecked()) {
        ui->search_output_ple->clear();
    }

    QStringList helpList {
        QString("%1%2%3")
        .arg(ui->category_combx->currentText())
            .arg("=")
            .arg(ui->param_combox->currentText())
    };

    ui->search_output_ple->setPlainText(m_probe.getHelp(helpList));
}


void HelpQueryWg::on_category_combx_activated(int index)
{
    if (QStringList{"full", "long"}.contains( ui->category_combx->currentText())) {
        ui->param_combox->setVisible(false);
    } else {
        ui->param_combox->setVisible(true);
    }

    ui->param_combox->clear();
    if (ui->category_combx->currentText() == DECODER_FMT) {
        ui->param_combox->addItems(m_probe.getCodecsFromLibav(CODEC_TYPE_DECODER));
        return;
    }
    if (ui->category_combx->currentText() == ENCODER_FMT) {
        ui->param_combox->addItems(m_probe.getCodecsFromLibav(CODEC_TYPE_ENCODER));
        return;
    }
    if (ui->category_combx->currentText() == DEMUXER_FMT) {
        ui->param_combox->addItems(m_probe.getMuxersFromLibav(MUXER_TYPE_DEMUXER));
        return;
    }
    if (ui->category_combx->currentText() == MUXER_FMT) {
        ui->param_combox->addItems(m_probe.getMuxersFromLibav(MUXER_TYPE_MUXER));
        return;
    }
    if (ui->category_combx->currentText() == FILTER_FMT) {
        ui->param_combox->addItems(m_probe.getFiltersFromLibav());
        return;
    }
    if (ui->category_combx->currentText() == BSF_FMT) {
        ui->param_combox->addItems(m_probe.getBsfFromLibav());
        return;
    }
    if (ui->category_combx->currentText() == PROTOCOL_FMT) {
        ui->param_combox->addItems(m_probe.getProtocolFromLibav());
        return;
    }
}


void HelpQueryWg::on_search_input_le_editingFinished()
{
    emit ui->search_btn->clicked();
}

