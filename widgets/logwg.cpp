#include "logwg.h"
#include "ui_logwg.h"

LogWG::LogWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogWG)
{
    ui->setupUi(this);
    ui->end_time_dte->setVisible(false);
}

LogWG::~LogWG()
{
    delete ui;
}

void LogWG::outLog(const QString &log)
{
    ui->log_ple->appendPlainText(log);
}

void LogWG::on_expand_time_btn_clicked()
{
    ui->end_time_dte->setVisible(ui->expand_time_btn->isChecked());
}

