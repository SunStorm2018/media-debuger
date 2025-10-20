#ifndef GLOBALCONFINGWG_H
#define GLOBALCONFINGWG_H

#include <QWidget>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QButtonGroup>
#include <QPushButton>

#include <widgets/infotablewg.h>

namespace Ui {
class GlobalConfingWG;
}

class GlobalConfingWG : public QWidget
{
    Q_OBJECT

public:
    explicit GlobalConfingWG(QWidget *parent = nullptr);
    ~GlobalConfingWG();

    void setCurrentConfig(const QString& group);

private:
    Ui::GlobalConfingWG *ui;

    InfoWidgets *generalCfgWg;
    QButtonGroup *configButtonGroup;

private:
    void loadConfigData(const QString& group, const QStringList& keys = QStringList());
    void setupButtonGroup();
};

#endif // GLOBALCONFINGWG_H
