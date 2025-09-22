#include "helpquerywg.h"
#include "ui_helpquerywg.h"

HelpQueryWg::HelpQueryWg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpQueryWg)
{
    ui->setupUi(this);
    ui->category_combx->addItems(HELP_OPTION_FORMATS);

    qDebug() << "get codecs-encoder:" << m_probe.getCodecsFromLibav(CODEC_TYPE_ENCODER);
    qDebug() << "get codecs-decoder:" << m_probe.getCodecsFromLibav(CODEC_TYPE_DECODER);
    qDebug() << "get muxer-demuxer:" << m_probe.getMuxersFromLibav(MUXER_TYPE_DEMUXER);
    qDebug() << "get muxer-muxer:" << m_probe.getMuxersFromLibav(MUXER_TYPE_MUXER);
    qDebug() << "get filters:" << m_probe.getFiltersFromLibav();
    qDebug() << "get bsf:" << m_probe.getBsfFromLibav();
    qDebug() << "get protocols:" << m_probe.getProtocolFromLibav();
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

    m_probe.getHelp(helpList);
}


void HelpQueryWg::on_category_combx_activated(int index)
{

}


void HelpQueryWg::on_search_input_le_editingFinished()
{
    emit ui->search_btn->clicked();
}

