#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("FFprobe Viewer");

    ZWindowHelper::centerToCurrentScreen(this);
    setAttribute(Qt::WA_QuitOnClose, true);

    InitConnectation();

    // Create DockWidget layout
    createDockWidgets();

    // Restore layout settings
    restoreLayoutSettings();

    // Set window title
    setWindowTitle(APPLICATION_NAME);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::InitConnectation()
{
    connect(ui->menuFile, &QMenu::triggered, this, &MainWindow::slotMenuFileTriggered);
    connect(ui->menuBasic_Info, &QMenu::triggered, this, &MainWindow::slotMenuBasic_InfoTriggered);
    connect(ui->menuMedia_Info, &QMenu::triggered, this, &MainWindow::slotMenuMedia_InfoTriggered);
    connect(ui->menuConfig, &QMenu::triggered, this, &MainWindow::slotMenuConfigTriggered);
    connect(ui->menuHelp, &QMenu::triggered, this, &MainWindow::slotMenuHelpTriggered);
    connect(ui->menuPlay, &QMenu::triggered, this, &MainWindow::slotMenuPlayTriggered);

    connect(ZLogger::instance(), &ZLogger::logMessage, &m_logWG, &LogWG::outLog);
}

void MainWindow::PopBasicInfoWindow(QString title, const QString &info, const QString& format_key)
{
    InfoWidgets *infoWindow = new InfoWidgets;
    infoWindow->setObjectName(title.replace(" ", "") + "Wg");
    infoWindow->setAttribute(Qt::WA_DeleteOnClose);

    infoWindow->setWindowTitle(title);
    infoWindow->show();
    ZWindowHelper::centerToParent(infoWindow);
    infoWindow->init_detail_tb(info, format_key.toLower());

    qDebug() << title << info;

}

void MainWindow::PopMediaInfoWindow(QString title, const QString &info, const QString &format_key)
{
    BaseFormatWG *mediaInfoWindow = nullptr;
    if (format_key == "json") {
        mediaInfoWindow = new JsonFormatWG;
    } else if (format_key == "table") {
        mediaInfoWindow = new TabelFormatWG;
    }

    if (mediaInfoWindow == nullptr)
        return;

    mediaInfoWindow->setWindowTitle(title);
    mediaInfoWindow->setAttribute(Qt::WA_DeleteOnClose);
    mediaInfoWindow->show();
    ZWindowHelper::centerToParent(mediaInfoWindow);
    mediaInfoWindow->loadJson(info.toUtf8());

    qDebug() << title << info.size();
}

void MainWindow::createDockWidgets()
{
    // Create FilesWG DockWidget (left side)
    m_filesWGDock = new QDockWidget(tr("Files"), this);
    m_filesWGDock->setObjectName("FilesDock");
    m_filesWGDock->setWidget(&m_filesWG);
    addDockWidget(Qt::LeftDockWidgetArea, m_filesWGDock);

    // Create LogWG DockWidget (bottom)
    m_logWGDock = new QDockWidget(tr("Logs"), this);
    m_logWGDock->setObjectName("LogsDock");
    m_logWGDock->setWidget(&m_logWG);
    addDockWidget(Qt::BottomDockWidgetArea, m_logWGDock);

    // Set PlayerWG as central widget (occupies remaining space)
    m_playerWGDock = new QDockWidget(tr("Player"), this);
    m_playerWGDock->setObjectName("PlayerDock");
    m_playerWGDock->setWidget(&m_playerWG);
    addDockWidget(Qt::RightDockWidgetArea, m_playerWGDock);

    // Set dock widget sizes
    m_filesWGDock->setMinimumWidth(200);
    m_logWGDock->setMinimumHeight(150);
    m_playerWGDock->setMinimumWidth(300);
}

void MainWindow::saveLayoutSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    // Save window geometry
    settings.setValue(GEOMETRY_KEY, saveGeometry());

    // Save dock layout state
    settings.setValue(STATE_KEY, saveState());

    qInfo() << "Window layout settings saved";
}

void MainWindow::restoreLayoutSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);

    // Restore window geometry
    if (settings.contains(GEOMETRY_KEY)) {
        restoreGeometry(settings.value(GEOMETRY_KEY).toByteArray());
    }

    // Restore dock layout state
    if (settings.contains(STATE_KEY)) {
        restoreState(settings.value(STATE_KEY).toByteArray());
        qInfo() << "Window layout restored from settings";
    } else {
        // Default layout for first run
        resize(1200, 800);
        qInfo() << "Using default window layout";
    }
}


void MainWindow::slotMenuBasic_InfoTriggered(QAction *action)
{
    if (!action) {
        return;
    }

    QString function = action->objectName().replace("action", "get");
    QString retVal;
    QMetaObject::invokeMethod(&m_probe, function.toUtf8(), Qt::DirectConnection,
                              Q_RETURN_ARG(QString, retVal));

    PopBasicInfoWindow(action->objectName().replace("action", "Detail Info : "), retVal, action->objectName().replace("action", ""));
}

void MainWindow::slotMenuMedia_InfoTriggered(QAction *action)
{
    if (!action) {
        return;
    }

    QString function = action->objectName().replace("action", "-show_").toLower();

    if (QStringList{
            SHOW_FORMAT, SHOW_STREAMS, SHOW_CHAPTERS,
            SHOW_FRAMES, SHOW_PACKETS,
            SHOW_PROGRAMS,
            SHOW_VERSIONS, SHOW_PROGRAM_VERSION, SHOW_LIBRARY_VERSIONS,
            SHOW_PIXEL_FORMATS
        }.contains(function))
    {
        QString fileName = Common::instance()->getConfigValue(CURRENTFILE).toString();
        if (!fileName.isEmpty()) {
            QString formats = m_probe.getMediaInfoJsonFormat(function, fileName);
            PopMediaInfoWindow(action->objectName().replace("action", "Detail Info : "), formats);
        } else {
            qWarning() << CURRENTFILE << "is empty, please retray";
        }

        return;
    }

    if (QStringList{
            SHOW_FRAMES, SHOW_PACKETS,
            SHOW_FRAMES_VIDEO, SHOW_FRAMES_AUDIO,
            SHOW_PACKETS_VIDEO, SHOW_PACKETS_AUDIO
        }.contains(function))
    {
        QString tmpFunction = function;
        if (function.contains("audio",Qt::CaseInsensitive)){
            tmpFunction = tmpFunction.replace("_audio", "", Qt::CaseInsensitive);
            tmpFunction += tr(" %1 a:0").arg(SELECT_STREAMS);
        }
        if (function.contains("video",Qt::CaseInsensitive)){
            tmpFunction = tmpFunction.replace("_video", "", Qt::CaseInsensitive);
            tmpFunction += tr(" %1 v:0").arg(SELECT_STREAMS);
        }
        QString fileName = Common::instance()->getConfigValue(CURRENTFILE).toString();
        if (!fileName.isEmpty()) {
            QString formats = m_probe.getMediaInfoJsonFormat(tmpFunction, fileName);
            PopMediaInfoWindow(action->objectName().replace("action", "Detail Info : "), formats, "table");
        } else {
            qWarning() << CURRENTFILE << fileName  << "is empty, please retray";
        }

        return;
    }
}

void MainWindow::slotMenuFileTriggered(QAction *action)
{
    if (!action) {
        return;
    }

    if (ui->actionOpen == action) {
        QString fileName = QFileDialog::getOpenFileName(
            nullptr,
            "Open File",
            QDir::homePath(),
            "All Files (*.*)"
            );

        if (!fileName.isEmpty()) {
            qDebug() << "Selected file:" << fileName;
            Common::instance()->setConfigValue(CURRENTFILE, fileName);
        }

        return;
    }

    if (ui->actionOpen == action) {
        QStringList fileNames = QFileDialog::getOpenFileNames(
            nullptr,
            "Select Files",
            QDir::homePath(),
            "All Files (*.*);;Text Files (*.txt);;Image Files (*.png *.jpg *.bmp)"
            );

        if (!fileNames.isEmpty()) {
            for (const QString &fileName : fileNames) {
                qDebug() << "Selected file:" << fileName;
            }

            Common::instance()->setConfigValue(CURRENTFILES, fileNames);
        }

        return;
    }

    if (ui->actionFiles == action) {
        m_filesWGDock->show();
        m_filesWGDock->raise();
        return;
    }
}

void MainWindow::slotMenuConfigTriggered(QAction *action)
{
    if (!action) {
        return;
    }

    GlobalConfingWG *configWg = new GlobalConfingWG;
    ZWindowHelper::centerToParent(configWg, true);
    configWg->setCurrentTab(action->text());
    configWg->show();
    }

void MainWindow::slotMenuHelpTriggered(QAction *action)
{
    if (!action) {
        return;
    }
    if (ui->actionLog == action) {
        m_logWGDock->show();
        m_logWGDock->raise();
    }
    if (ui->actionAbout == action) {
        QMessageBox::about(this,
                           tr("About MediaDebuger"),
                           tr("MediaDebuger\n\n"
                              "A powerful media file analysis tool that provides:\n"
                              "- Detailed media information display\n"
                              "- Multiple format views (JSON, Table)\n"
                              "- Real-time logging\n"
                              "- Customizable layout\n\n"
                              "Version 1.0"));
    }
    if (ui->actionApp_Dir == action) {
        QString appDir = QCoreApplication::applicationDirPath();
        QDesktopServices::openUrl(QUrl::fromLocalFile(appDir));
    }
}



void MainWindow::slotMenuPlayTriggered(QAction *action)
{
    if (!action) {
        return;
    }

    if (ui->actionPlayer == action) {
        m_playerWGDock->show();
        m_playerWGDock->raise();
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveLayoutSettings();
    QMainWindow::closeEvent(event);
}
