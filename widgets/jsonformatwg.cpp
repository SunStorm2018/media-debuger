#include "jsonformatwg.h"
#include "ui_jsonformatwg.h"

JsonFormatWG::JsonFormatWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::JsonFormatWG)
{
    ui->setupUi(this);

    m_model = new QJsonModel;

    ui->treeView->setModel(m_model);
    ui->treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

JsonFormatWG::~JsonFormatWG()
{
    delete ui;
}

bool JsonFormatWG::loadJson(const QByteArray &json)
{
    bool res = m_model->loadJson(json);
    ui->treeView->expandAll();
    return res;
}
