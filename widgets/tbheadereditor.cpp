#include "tbheadereditor.h"
#include "ui_tbheadereditor.h"

TBHeaderEditor::TBHeaderEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TBHeaderEditor)
{
    ui->setupUi(this);
}

TBHeaderEditor::~TBHeaderEditor()
{
    delete ui;
}
