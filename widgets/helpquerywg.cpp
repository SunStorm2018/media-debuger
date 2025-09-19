#include "helpquerywg.h"
#include "ui_helpquerywg.h"

HelpQueryWg::HelpQueryWg(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpQueryWg)
{
    ui->setupUi(this);
    ui->category_combx->addItems(HELP_OPTION_FORMATS);

}

HelpQueryWg::~HelpQueryWg()
{
    delete ui;
}

bool HelpQueryWg::setHelpParams(const QString &category, const QString &value)
{
    ui->search_output_ple->clear();

    ZFfprobe probe;

    QStringList helpList {
        QString("%1%2%3")
        .arg(category)
            .arg("=")
            .arg(value)
    };

    qDebug() << probe.getHelp(helpList);
    ui->search_output_ple->setPlainText(probe.getHelp(helpList));
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

