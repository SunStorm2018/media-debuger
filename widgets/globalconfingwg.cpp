#include "globalconfingwg.h"
#include "ui_globalconfingwg.h"

GlobalConfingWG::GlobalConfingWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GlobalConfingWG)
{
    ui->setupUi(this);
    generalCfgWg = new InfoWidgets(this);

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
    QList<QStringList> data;

    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    for (int row = 0; row < settings.allKeys().size(); ++row) {
        QString key = settings.allKeys().at(row);
        QVariant value = settings.value(key);
        data << QStringList{key, value.toString()};
    }

    generalCfgWg->init_header_detail_tb(headers);
    generalCfgWg->update_data_detail_tb(data);

    ui->genreral_tab_layout->addWidget(generalCfgWg);
}
