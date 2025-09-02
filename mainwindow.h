#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QScreen>

#include <Common/zffprobe.h>
#include <Common/zwindowhelper.h>
#include <Widgets/infowidgets.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void InitConnectation();
    void PopInfoWindow(QString title, const QString &info, const QString &format_key);

private:
    Ui::MainWindow *ui;

    ZFfprobe m_probe;

    QList<QWidget *> m_infowindows;
};
#endif // MAINWINDOW_H
