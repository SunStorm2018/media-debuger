#include "globalconfingwg.h"
#include "ui_globalconfingwg.h"

GlobalConfingWG::GlobalConfingWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GlobalConfingWG)
{
    ui->setupUi(this);
    setWindowTitle("Config");
    loadGeneralConfig();
}

GlobalConfingWG::~GlobalConfingWG()
{
    delete ui;
}

void GlobalConfingWG::setCurrentTab(const QString &tabName)
{
    for (int i = 0; i < ui->config_tab_wg->count(); ++i) {
        if (ui->config_tab_wg->tabText(i).toLower() == tabName.toLower()) {
            ui->config_tab_wg->setCurrentIndex(i);
            break;
        }
    }
}

void GlobalConfingWG::loadGeneralConfig()
{
    QStringList headers{"Key", "Value"};
    ui->general_config_tb->setColumnCount(2);
    ui->general_config_tb->setHorizontalHeaderLabels(headers);
    ui->general_config_tb->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->general_config_tb->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);


    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QStringList allKeys = settings.allKeys();
    ui->general_config_tb->setRowCount(allKeys.size());
    for (int row = 0; row < allKeys.size(); ++row) {
        QString key = allKeys.at(row);
        QVariant value = settings.value(key);

        QTableWidgetItem *keyItem = new QTableWidgetItem(key);
        ui->general_config_tb->setItem(row, 0, keyItem);

        QTableWidgetItem *valueItem = new QTableWidgetItem(value.toString());
        ui->general_config_tb->setItem(row, 1, valueItem);
    }
}
