#include "frameinfowidgets.h"
#include "ui_frameinfowidgets.h"

frameInfowidgets::frameInfowidgets(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::frameInfowidgets)
{
    ui->setupUi(this);
}

frameInfowidgets::~frameInfowidgets()
{
    delete ui;
}
