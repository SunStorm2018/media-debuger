#include "fileswg.h"
#include "ui_fileswg.h"

FilesWG::FilesWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FilesWG)
{
    ui->setupUi(this);
}

FilesWG::~FilesWG()
{
    delete ui;
}
