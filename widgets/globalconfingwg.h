#ifndef GLOBALCONFINGWG_H
#define GLOBALCONFINGWG_H

#include <QWidget>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>

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

private:
    void loadGeneralConfig();
};

#endif // GLOBALCONFINGWG_H
