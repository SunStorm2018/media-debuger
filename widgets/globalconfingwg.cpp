#include "globalconfingwg.h"
#include "ui_globalconfingwg.h"

GlobalConfingWG::GlobalConfingWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GlobalConfingWG)
{
    ui->setupUi(this);
    generalCfgWg = new InfoWidgets(this);

    setWindowTitle("Global Config");
    setupButtonGroup();
    
    ui->genreral_tab_layout->addWidget(generalCfgWg);

    connect(generalCfgWg, &InfoWidgets::dataChanged, [=](QStringList configs) {
        if (configs.size() == 2) {
            QStringList settingParts = configs.at(0).split("/", QString::SkipEmptyParts);
            if (settingParts.size() == 2) {
                QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
                settings.beginGroup(settingParts.at(0));
                settings.setValue(settingParts.at(1), configs.at(1));
                settings.endGroup();
            }
        }
    });
}

GlobalConfingWG::~GlobalConfingWG()
{
    delete ui;
}

void GlobalConfingWG::setCurrentConfig(const QString &group)
{
    loadConfigData(group);
}

void GlobalConfingWG::setupButtonGroup()
{
    configButtonGroup = new QButtonGroup(this);
    configButtonGroup->setExclusive(true);
    for (auto it : CONFIG_GROUPS) {
        QPushButton * btn = new QPushButton(it, this);
        connect(btn, &QPushButton::clicked, [=](){
            loadConfigData(btn->text());
        });
        ui->config_category_group->addWidget(btn);
    }

}


void GlobalConfingWG::loadConfigData(const QString& group, const QStringList& keys)
{
    QStringList headers{"Key", "Value"};
    QList<QStringList> data;

    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    
    if (!group.isEmpty()) {
        settings.beginGroup(group);
    }

    QStringList keysToLoad = keys;
    if (keysToLoad.isEmpty()) {
        keysToLoad = settings.allKeys();
    }

    for (const QString &key : keysToLoad) {
        if (settings.contains(key)) {
            QVariant value = settings.value(key);
            QString displayKey = group.isEmpty() ? key : group + "/" + key;
            data << QStringList{displayKey, value.toString()};
        }
    }

    if (!group.isEmpty()) {
        settings.endGroup();
    }

    generalCfgWg->init_header_detail_tb(headers);
    generalCfgWg->update_data_detail_tb(data, "=");

}
