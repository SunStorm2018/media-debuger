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

    for (auto it : CONFIG_GROUPS) {
        auto action = new QAction(it, ui->menuConfig);
        action->setObjectName(it);
        ui->menuConfig->addAction(action);
    }

    InitConnectation();

    // Create DockWidget layout
    createDockWidgets();

    // Restore layout settings
    restoreLayoutSettings();

    // Set window title
    setWindowTitle(APPLICATION_NAME);

    m_filesWG.addActions(getFilesAvailableAction());
    m_filesWG.addSubActions("Media Info", getMediaInfoAvailableActions());
    m_filesWG.addMenus(getMediaInfoAvailableMenus());

    if (m_filesWG.getCurrentSelectFileName().isEmpty()) {
        m_playerWG.setMediaFile(Common::instance()->getConfigValue(CURRENTFILE).toString());
    } else {
        m_playerWG.setMediaFile(m_filesWG.getCurrentSelectFileName());
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<QAction *> MainWindow::getMediaInfoAvailableActions()
{
    QList<QAction *> tmpActions;

    tmpActions << ui->actionStreams;
    tmpActions << ui->actionFormat;
    tmpActions << ui->actionChapters;
    // tmpActions << ui->actionFrames_Video;
    // tmpActions << ui->actionFrames_Audio;
    // tmpActions << ui->actionPackets_Video;
    // tmpActions << ui->actionPackets_Audio;

    return tmpActions;
}

QList<QAction *> MainWindow::getFilesAvailableAction()
{
    QList<QAction *> tmpActions;

    tmpActions << ui->actionOpen;
    tmpActions << ui->actionOpen_Files;
    tmpActions << ui->actionOpen_Folder;
    tmpActions << ui->actionExport;

    return tmpActions;
}

QList<QMenu *> MainWindow::getMediaInfoAvailableMenus()
{
    QList<QMenu *> tmpMenus;

    tmpMenus << ui->menuFrames;
    tmpMenus << ui->menuPackets;

    return tmpMenus;
}

void MainWindow::InitConnectation()
{
    // menu connection
    connect(ui->menuFile, &QMenu::triggered, this, &MainWindow::slotMenuFileTriggered);
    connect(ui->menuBasic_Info, &QMenu::triggered, this, &MainWindow::slotMenuBasic_InfoTriggered);
    connect(ui->menuConfig, &QMenu::triggered, this, &MainWindow::slotMenuConfigTriggered);
    connect(ui->menuHelp, &QMenu::triggered, this, &MainWindow::slotMenuHelpTriggered);
    connect(ui->menuPlay, &QMenu::triggered, this, &MainWindow::slotMenuPlayTriggered);

    // media info
    QList<QAction*> mediaActions;
    getMenuAllActions(ui->menuMedia_Info, mediaActions);
    for (auto action : mediaActions){
        connect(action, &QAction::triggered, this, &MainWindow::slotMenuMedia_InfoTriggered);
    }

    // log
    connect(ZLogger::instance(), &ZLogger::logMessage, &m_logWG, &LogWG::outLog);

    connect(&m_filesWG, &FilesWG::currentFileActived, [=](QPair<QString, QString> filePair){
        m_playerWG.setMediaFile(filePair.second);
    });
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

    // fit help option
    infoWindow->setHelpInfoKey(format_key.mid(0, format_key.length() - 1).toLower());

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

    // Create FoldersWG DockWidget (under FilesWG)
    m_foldersWGDock = new QDockWidget(tr("Folders"), this);
    m_foldersWGDock->setObjectName("FoldersDock");
    m_foldersWGDock->setWidget(&m_foldersWG);
    addDockWidget(Qt::LeftDockWidgetArea, m_foldersWGDock);

    // Position FoldersWG under FilesWG
    splitDockWidget(m_filesWGDock, m_foldersWGDock, Qt::Vertical);

    // Set PlayerWG as central widget (occupies remaining space)
    m_playerWGDock = new QDockWidget(tr("Player"), this);
    m_playerWGDock->setObjectName("PlayerDock");
    m_playerWGDock->setWidget(&m_playerWG);
    addDockWidget(Qt::RightDockWidgetArea, m_playerWGDock);

    // Set dock widget sizes
    m_filesWGDock->setMinimumWidth(200);
    m_foldersWGDock->setMinimumWidth(200);
    m_logWGDock->setMinimumHeight(150);
    m_playerWGDock->setMinimumWidth(300);
}

void MainWindow::saveLayoutSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.beginGroup(MAINWINDOW_SETTINGS_GROUP);
    // Save window geometry
    settings.setValue(GEOMETRY_KEY, saveGeometry());

    // Save dock layout state
    settings.setValue(STATE_KEY, saveState());
    settings.endGroup();
    qInfo() << "Window layout settings saved";
}

void MainWindow::restoreLayoutSettings()
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.beginGroup(MAINWINDOW_SETTINGS_GROUP);
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
    settings.endGroup();
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

    PopBasicInfoWindow(action->objectName().replace("action", "Detail Info : ") + m_filesWG.getCurrentSelectFileName(),
                       retVal,
                       action->objectName().replace("action", ""));
}

void MainWindow::slotMenuMedia_InfoTriggered(bool checked)
{
    Q_UNUSED(checked);

    QAction *senderAction = static_cast<QAction*>(QObject::sender());

    if (!senderAction) {
        return;
    }

    QString function = senderAction->objectName().replace("action", "-show_").toLower();
    if (function.isEmpty()) {
        function = QObject::sender()->objectName();
    }

    if (QStringList{
            SHOW_FORMAT, SHOW_STREAMS, SHOW_CHAPTERS,
            SHOW_FRAMES, SHOW_PACKETS,
            SHOW_PROGRAMS,
            SHOW_VERSIONS, SHOW_PROGRAM_VERSION, SHOW_LIBRARY_VERSIONS,
            SHOW_PIXEL_FORMATS
        }.contains(function))
    {
        QString fileName = m_filesWG.getCurrentSelectFileName();
        if (!fileName.isEmpty()) {
            QString formats = m_probe.getMediaInfoJsonFormat(function, fileName);
            PopMediaInfoWindow(senderAction->objectName().replace("action", "Detail Info : ") + m_filesWG.getCurrentSelectFileName(),
                               formats);
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
        QString fileName = m_filesWG.getCurrentSelectFileName();
        if (!fileName.isEmpty()) {
            ProgressDialog *progressDlg = new ProgressDialog;
            progressDlg->setWindowTitle(tr("Parse Media: %1").arg(fileName));
            progressDlg->setProgressMode(ProgressDialog::Indeterminate);
            progressDlg->setMessage("Parsing...");
            progressDlg->setAutoClose(true);

            progressDlg->start();
            QtConcurrent::run([=](){
                QString formats = m_probe.getMediaInfoJsonFormat(tmpFunction, fileName);
                QMetaObject::invokeMethod(this, "PopMediaInfoWindow",
                                          Qt::QueuedConnection,
                                          Q_ARG(QString, senderAction->objectName().replace("action", "Detail Info : ")),
                                          Q_ARG(QString, formats),
                                          Q_ARG(QString, "table")
                                          );
                progressDlg->setMessage("Finsh parse");
                progressDlg->finish();
                progressDlg->deleteLater();
            });
            progressDlg->exec();
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
            m_filesWG.addFileToHistory(fileName);
        }

        return;
    }

    if (ui->actionOpen_Files == action) {
        QStringList fileNames = QFileDialog::getOpenFileNames(
            nullptr,
            "Select Files",
            QDir::homePath(),
            "All Files (*.*);;Media Files (*.mp4 *.mkv *.webm *.mp3);;Image Files (*.png *.jpg *.bmp)"
            );

        if (!fileNames.isEmpty()) {
            for (const QString &fileName : fileNames) {
                m_filesWG.addFileToHistory(fileName);
                qDebug() << "Selected file:" << fileName;
            }

            Common::instance()->setConfigValue(CURRENTFILES, fileNames);
        }

        return;
    }
    if (ui->actionExport == action) {
        ExportWG *exportDlg = new ExportWG;
        exportDlg->setWindowTitle(tr("Export Files"));
        exportDlg->setAttribute(Qt::WA_DeleteOnClose);
        exportDlg->setExportFiledsOptions(QStringList{
            SHOW_FORMAT, SHOW_STREAMS, SHOW_CHAPTERS,
            SHOW_FRAMES, SHOW_PACKETS,
            SHOW_PROGRAMS,
            SHOW_VERSIONS, SHOW_PROGRAM_VERSION, SHOW_LIBRARY_VERSIONS,
            SHOW_PIXEL_FORMATS
        });
        exportDlg->setInputMediaFilePath(m_filesWG.getCurrentSelectFileName());
        exportDlg->show();
        ZWindowHelper::centerToParent(exportDlg);
    }

}

void MainWindow::slotMenuConfigTriggered(QAction *action)
{
    if (!action) {
        return;
    }

    GlobalConfingWG *configWg = new GlobalConfingWG;
    ZWindowHelper::centerToParent(configWg, true);
    configWg->setCurrentConfig(action->objectName());
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

        return;
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

    if (ui->actionSetting_Dir == action) {
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        QString fileName = settings.fileName();

        QFileInfo fileInfo(fileName);
        if (fileInfo.exists()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
        } else {
            qWarning() << "Setting dir not exists!";
        }
    }

    if (ui->actionSetting_File == action) {
        QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
        QString fileName = settings.fileName();

        QFile file(fileName);
        if (file.exists()) {
            QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
        } else {
            qWarning() << "Setting File not exists!";
        }
    }

    if (ui->actionHelp == action) {
        HelpQueryWg *helpWindow = new HelpQueryWg;
        helpWindow->setAttribute(Qt::WA_DeleteOnClose);

        helpWindow->setWindowTitle("Help Query");
        helpWindow->show();
        ZWindowHelper::centerToParent(helpWindow);
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

void MainWindow::getMenuAllActions(QMenu *menu, QList<QAction *> &actionList)
{
    if (!menu) return;

    QList<QAction*> actions = menu->actions();

    for (QAction* action : actions) {
        if (action->isSeparator()) {
            continue;
        }

        if (QMenu* subMenu = action->menu()) {
            getMenuAllActions(subMenu, actionList);
        } else {
            if (!actionList.contains(action)) {
                actionList.append(action);
            }
        }
    }
}

QList<QAction *> MainWindow::getMenuAllActions(QMenu *menu)
{
    QList<QAction *> actionList;

    getMenuAllActions(menu, actionList);

    return actionList;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveLayoutSettings();
    QMainWindow::closeEvent(event);
}
