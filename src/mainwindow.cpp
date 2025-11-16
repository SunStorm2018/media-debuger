// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QFileInfo>
#include <QTabWidget>

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
    
    // If no file is selected in FilesWG, try to get from config
    QString currentFile = m_filesWG.getCurrentSelectFileName();
    if (currentFile.isEmpty()) {
        currentFile = Common::instance()->getConfigValue(CURRENTFILE).toString();
        // If we got a file from config, set it in the player
        if (!currentFile.isEmpty()) {
            m_playerWG.setMediaFile(currentFile);
        }
    } else {
        // If FilesWG has a selection, set it in the player
        m_playerWG.setMediaFile(currentFile);
    }
    
    // Load media properties for the current file on startup
    if (!currentFile.isEmpty() && QFile::exists(currentFile)) {
        loadMediaProperties(currentFile);
    }

    m_actionWidgetMap = {
        {ui->actionPlayer, m_playerWGDock},
        {ui->actionLog, m_logWGDock},
        {ui->actionFiles, m_filesWGDock},
        {ui->actionFolders, m_foldersWGDock},
        {ui->actionMedia_Properties, m_mediaPropsWidget}
    };
}

MainWindow::~MainWindow()
{
    delete ui;
}

QList<QAction *> MainWindow::getMediaInfoAvailableActions()
{
    QList<QAction *> tmpActions;

    tmpActions << ui->actionShow_Streams;
    tmpActions << ui->actionShow_Format;
    tmpActions << ui->actionShow_Chapters;

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

void MainWindow::showBasicInfo(const QString &function, const QString &windwowTitle, const QString &formatKey)
{
    bool sucess = false;
    QString retVal = m_probe.getBasicInfo(function, &sucess);

    if (!sucess) {
        qWarning() << tr("Basic info get error");
        return;
    }
    PopBasicInfoWindow(windwowTitle,
                       retVal,
                       formatKey);
}

void MainWindow::showMediaInfo(const QString fileName, const QString &function, const QString &windwowTitle, const QString &formatKey)
{
    ProgressDialog *progressDlg = new ProgressDialog;
    progressDlg->setWindowTitle(tr("Parse Media: %1").arg(fileName));
    progressDlg->setProgressMode(ProgressDialog::Indeterminate);
    progressDlg->setMessage("Parsing...");
    progressDlg->setAutoClose(true);

    progressDlg->start();
    QtConcurrent::run([=](){
        QString formats = m_probe.getMediaInfoJsonFormat(function, fileName);
        bool ok = QMetaObject::invokeMethod(this, "PopMediaInfoWindow",
                                  Qt::QueuedConnection,
                                  Q_ARG(QString, windwowTitle),
                                  Q_ARG(QString, formats),
                                  Q_ARG(QString, formatKey)
                                  );
        qDebug() << "Media info query: " << ok;
        emit progressDlg->messageChanged("Finsh parse");
        emit progressDlg->toFinish();
        progressDlg->deleteLater();
    });
    progressDlg->exec();
}

void MainWindow::InitConnectation()
{
    // menu connection
    connect(ui->menuFile, &QMenu::triggered, this, &MainWindow::slotMenuFileTriggered);
    connect(ui->menuBasic_Info, &QMenu::triggered, this, &MainWindow::slotMenuBasic_InfoTriggered);
    connect(ui->menuConfig, &QMenu::triggered, this, &MainWindow::slotMenuConfigTriggered);
    connect(ui->menuHelp, &QMenu::triggered, this, &MainWindow::slotMenuHelpTriggered);
    connect(ui->menuPlay, &QMenu::triggered, this, &MainWindow::slotMenuPlayTriggered);
    connect(ui->menuView, &QMenu::triggered, this, &MainWindow::slotMenuViewTriggered);

    connect(ui->menuView, &QMenu::aboutToShow, this, &MainWindow::slotMenuViewAboutToShow);

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
        
        // Update media properties dock if exists
        loadMediaProperties(filePair.second);
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
    if (format_key == FORMAT_JSON) {
        mediaInfoWindow = new JsonFormatWG;
    } else if (format_key == FORMAT_TABLE) {
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

void MainWindow::PopMediaPropsWindow(const QString &fileName)
{
    // Update content if a valid file is provided
    if (!fileName.isEmpty() && m_mediaPropsWidget) {
        loadMediaProperties(fileName);
        
        // Update window title to reflect current file
        setWindowTitle(tr("%1 - Media Properties: %2").arg(APPLICATION_NAME, QFileInfo(fileName).fileName()));
    }
}

void MainWindow::loadMediaProperties(const QString &fileName)
{
    if (!fileName.isEmpty() && m_mediaPropsWidget) {
        // Check if file exists before processing
        if (!QFile::exists(fileName)) {
            qWarning() << "Media file does not exist:" << fileName;
            return;
        }
        
        // Update dock title
        m_mediaPropsWGDock->setWindowTitle(tr("Properties: %1").arg(QFileInfo(fileName).fileName()));
        
        // Update Format tab
        JsonFormatWG *formatWidget = qobject_cast<JsonFormatWG*>(m_mediaPropsWidget->widget(0));
        if (formatWidget) {
            QString formatInfo = m_probe.getMediaInfoJsonFormat(SHOW_FORMAT, fileName);
            formatWidget->loadData(formatInfo.toUtf8());
        }
        
        // Update Streams tab
        JsonFormatWG *streamsWidget = qobject_cast<JsonFormatWG*>(m_mediaPropsWidget->widget(1));
        if (streamsWidget) {
            QString streamsInfo = m_probe.getMediaInfoJsonFormat(SHOW_STREAMS, fileName);
            streamsWidget->loadData(streamsInfo.toUtf8());
        }
    }
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

    // Create Media Properties as central widget (center area)
    m_mediaPropsWidget = new QTabWidget;
    m_mediaPropsWidget->setObjectName("MediaPropsTabWidget");
    
    // Create a widget with layout to manage margins
    QWidget *centralWidget = new QWidget;
    QVBoxLayout *centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setContentsMargins(9, 9, 9, 9);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(m_mediaPropsWidget);
    
    // Create Format tab
    JsonFormatWG *formatWidget = new JsonFormatWG;
    formatWidget->setObjectName("FormatWidget");
    m_mediaPropsWidget->addTab(formatWidget, tr("Format"));
    
    // Create Streams tab
    JsonFormatWG *streamsWidget = new JsonFormatWG;
    streamsWidget->setObjectName("StreamsWidget");
    m_mediaPropsWidget->addTab(streamsWidget, tr("Streams"));
    
    // Create Media Properties DockWidget (for visibility control)
    m_mediaPropsWGDock = new QDockWidget(tr("Media Properties"), this);
    m_mediaPropsWGDock->setObjectName("MediaPropsDock");
    m_mediaPropsWGDock->setWidget(centralWidget);
    setCentralWidget(m_mediaPropsWGDock);

    // Set PlayerWG as dock widget (right side)
    m_playerWGDock = new QDockWidget(tr("Player"), this);
    m_playerWGDock->setObjectName("PlayerDock");
    m_playerWGDock->setWidget(&m_playerWG);
    addDockWidget(Qt::RightDockWidgetArea, m_playerWGDock);

    // Set dock widget sizes
    m_filesWGDock->setMinimumWidth(200);
    m_foldersWGDock->setMinimumWidth(200);
    m_logWGDock->setMinimumHeight(150);
    m_playerWGDock->setMinimumWidth(400);
    m_mediaPropsWGDock->setMinimumWidth(300);
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

    showBasicInfo(function,
                  action->objectName().replace("action", "Detail Info : ") + m_filesWG.getCurrentSelectFileName(),
                  action->objectName().replace("action", ""));
}

void MainWindow::slotMenuMedia_InfoTriggered(bool checked)
{
    Q_UNUSED(checked);

    QAction *senderAction = static_cast<QAction*>(QObject::sender());

    if (!senderAction) {
        return;
    }

    QString function = senderAction->objectName().replace("action", "-").toLower();
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

    // Handle Media Properties action
    if (senderAction == ui->actionMedia_Properties) {
        QString fileName = m_filesWG.getCurrentSelectFileName();
        if (!fileName.isEmpty()) {
            // Just make the dock widget visible and update its content
            PopMediaPropsWindow(fileName);
        } else {
            qWarning() << CURRENTFILE << "is empty, please retry";
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
            showMediaInfo(fileName, tmpFunction, senderAction->objectName().replace("action", "Detail Info : "), FORMAT_TABLE);
        } else {
            qWarning() << CURRENTFILE << fileName  << "is empty, please retray";
        }

        return;
    }

    if (QStringList{
            COUNT_FRAMES, COUNT_PACKETS
        }.contains(function)) {

        QString tmpFunction = function;

        if (function == COUNT_FRAMES) {
            tmpFunction = "-count_frames -show_entries stream=index,codec_type,nb_read_frames,nb_frames";
        }

        if (function == COUNT_PACKETS) {
            tmpFunction = "-count_packets -show_entries stream=index,codec_type,nb_packets,nb_read_packets";
        }

        QString fileName = m_filesWG.getCurrentSelectFileName();

        if (!fileName.isEmpty()) {
            showMediaInfo(fileName, tmpFunction, senderAction->objectName().replace("action", "Detail Info : "), FORMAT_JSON);
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

        if (m_filesWG.getCurrentSelectFileName().isEmpty()) {
            qWarning() << tr("No input file.");
            exportDlg->setExportModel(BasicInfo);
        } else {
            exportDlg->setExportModel(ExportModelType(BasicInfo | MediaInfo));

            exportDlg->setMediaInfoExportFiledsOptions(QStringList{
                SHOW_FORMAT, SHOW_STREAMS, SHOW_CHAPTERS,
                SHOW_FRAMES, SHOW_PACKETS,
                SHOW_PROGRAMS,
                SHOW_VERSIONS, SHOW_PROGRAM_VERSION, SHOW_LIBRARY_VERSIONS,
                SHOW_PIXEL_FORMATS
            });
        }

        exportDlg->setBasicInfoExportFiledsOptions(QStringList{
            VERSION, BUILDCONF, FORMATS, MUXERS, DEMUXERS,
            DEVICES, CODECS, DECODERS, ENCODERS, BSFS,
            PROTOCOLS, FILTERS, PIX_FMTS, LAYOUTS,
            SAMPLE_FMTS, COLORS
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

    if (ui->actionAbout == action) {
        QMessageBox::about(this,
                           tr("About MediaDebuger"),
                           tr("MediaDebuger\n\n"
                              "A powerful media file analysis tool that provides:\n"
                              "- Detailed media information display\n"
                              "- Multiple format views (JSON, Table)\n"
                              "- Real-time logging\n"
                              "- Customizable layout\n"
                              "- FFmpeg compilation tool\n\n"
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

    if (ui->actionFFmpeg_Builder == action) {
        FFmpegBuildTool *ffbuilder = new FFmpegBuildTool;
        ffbuilder->setAttribute(Qt::WA_DeleteOnClose);

        ffbuilder->setWindowTitle("FFmpeg Builder");
        ffbuilder->setMinimumSize(1200, 900);
        ffbuilder->show();
        ZWindowHelper::centerToParent(ffbuilder);
    }
}

void MainWindow::slotMenuPlayTriggered(QAction *action)
{
    if (!action) {
        return;
    }
}

void MainWindow::slotMenuViewTriggered(QAction *action)
{
    if (!action) {
        return;
    }

    QWidget *wg = m_actionWidgetMap.value(action);
    if (wg) {
        if (action->isChecked()) {
            wg->show();
            wg->raise();
        } else {
            wg->hide();
        }
    }
    
    // Special handling for Media Properties since it's a central widget
    if (action == ui->actionMedia_Properties) {
        if (action->isChecked()) {
            m_mediaPropsWidget->show();
        } else {
            m_mediaPropsWidget->hide();
        }
    }
}

void MainWindow::slotMenuViewAboutToShow()
{
    for (auto action : getMenuAllActions(ui->menuView)) {
        action->setCheckable(true);
        action->setChecked(false);
        QWidget *wg = m_actionWidgetMap.value(action);
        if (wg && wg->isVisible()) {
            action->setChecked(true);
        }
    }
    
    // Special handling for Media Properties since it's a central widget
    ui->actionMedia_Properties->setChecked(m_mediaPropsWidget && m_mediaPropsWidget->isVisible());
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
