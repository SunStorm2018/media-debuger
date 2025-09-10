#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QScreen>
#include <QFileDialog>
#include <QDockWidget>

#include <common/zffprobe.h>
#include <common/zwindowhelper.h>
#include <common/common.h>
#include <common/zlogger.h>

#include <widgets/infowidgets.h>
#include <widgets/jsonformatwg.h>
#include <widgets/globalconfingwg.h>
#include <widgets/tabelformatwg.h>
#include <widgets/logwg.h>
#include <widgets/fileswg.h>
#include <widgets/playerwg.h>

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
    void PopMediaInfoWindow(QString title, const QString &info, const QString &format_key = "json");

    void createDockWidgets();
    void saveLayoutSettings();
    void restoreLayoutSettings();

private slots:
    void slotMenuBasic_InfoTriggered(QAction *action);
    void slotMenuMedia_InfoTriggered(QAction *action);
    void slotMenuFileTriggered(QAction *action);
    void slotMenuConfigTriggered(QAction *action);
    void slotMenuHelpTriggered(QAction *action);
    void slotMenuPlayTriggered(QAction *action);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;

    ZFfprobe m_probe;

    FilesWG &m_filesWG = FilesWG::instance();
    PlayerWG &m_playerWG = PlayerWG::instance();
    LogWG &m_logWG = LogWG::instance();

    QDockWidget *m_filesWGDock = nullptr;
    QDockWidget *m_logWGDock = nullptr;
    QDockWidget *m_playerWGDock = nullptr;

    QString GEOMETRY_KEY = "mainWindowGeometry";
    QString STATE_KEY = "mainWindowState";
};
#endif // MAINWINDOW_H
