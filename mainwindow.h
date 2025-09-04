#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QScreen>
#include <QFileDialog>


#include <common/zffprobe.h>
#include <common/zwindowhelper.h>
#include <common/common.h>

#include <widgets/infowidgets.h>
#include <widgets/jsonformatwg.h>
#include <widgets/globalconfingwg.h>

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
    void PopBasicInfoWindow(QString title, const QString &info, const QString &format_key);
    void PopMediaInfoWindow(QString title, const QString &info, const QString &format_key = "");

private slots:
    void slotMenuBasic_InfoTriggered(QAction *action);
    void slotMenuMedia_InfoTriggered(QAction *action);
    void slotMenuFileTriggered(QAction *action);
    void slotMenuConfigTriggered(QAction *action);

private:
    Ui::MainWindow *ui;

    ZFfprobe m_probe;

    QList<QWidget *> m_basiclInfowindows;
     QList<QWidget *> m_mediaInfowindows;
};
#endif // MAINWINDOW_H
