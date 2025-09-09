#ifndef GLOBALCONFINGWG_H
#define GLOBALCONFINGWG_H

#include <QWidget>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>

#include <widgets/infowidgets.h>

namespace Ui {
class GlobalConfingWG;
}

class GlobalConfingWG : public QWidget
{
    Q_OBJECT

public:
    explicit GlobalConfingWG(QWidget *parent = nullptr);
    ~GlobalConfingWG();

    void setCurrentTab(const QString& tabName);

private:
    Ui::GlobalConfingWG *ui;

    InfoWidgets *generalCfgWg;

private:
    void loadGeneralConfig();
};

#endif // GLOBALCONFINGWG_H
