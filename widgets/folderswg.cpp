#include "folderswg.h"
#include "ui_folderswg.h"

FoldersWG::FoldersWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FoldersWG)
{
    ui->setupUi(this);
}

FoldersWG::~FoldersWG()
{
    delete ui;
}
