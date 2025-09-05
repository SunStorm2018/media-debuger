#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QWidget>

namespace Ui {
class ConfigWindow;
}

class ConfigWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigWindow(QWidget *parent = nullptr);
    ~ConfigWindow();

private slots:
    void on_flags_gbox_clicked(bool checked);

private:
    Ui::ConfigWindow *ui;
};

#endif // CONFIGWINDOW_H
