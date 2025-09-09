#include "logwg.h"
#include "ui_logwg.h"

LogWG::LogWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogWG)
{
    ui->setupUi(this);
}

LogWG::~LogWG()
{
    delete ui;
}

void LogWG::outLog(const QString &log)
{

}
