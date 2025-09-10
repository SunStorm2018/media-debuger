#ifndef LOGWG_H
#define LOGWG_H

#include <QWidget>
#include <common/singleton.h>

namespace Ui {
class LogWG;
}

class LogWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_SINGLETON(LogWG)
    LogWG(QWidget *parent = nullptr);
    ~LogWG();

public slots:
    void outLog(const QString &log);

private slots:
    void on_expand_time_btn_clicked();
    void on_use_regular_express_cbx_clicked(bool checked);

private slots:
    void showContextMenu(const QPoint &pos);
    void toggleSearchDetail();
    void toggleView();

private:
    Ui::LogWG *ui;
};

#endif // LOGWG_H
