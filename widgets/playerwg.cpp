#include "playerwg.h"
#include "ui_playerwg.h"

PlayerWG::PlayerWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerWG)
{
    ui->setupUi(this);
}

PlayerWG::~PlayerWG()
{
    delete ui;
}
