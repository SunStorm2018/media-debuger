#ifndef LOGWG_H
#define LOGWG_H

#include <QWidget>

namespace Ui {
class LogWG;
}

class LogWG : public QWidget
{
    Q_OBJECT

public:
    explicit LogWG(QWidget *parent = nullptr);
    ~LogWG();

public slots:
    void outLog(const QString &log);

private slots:
    void on_expand_time_btn_clicked();

private:
    Ui::LogWG *ui;
};

#endif // LOGWG_H
