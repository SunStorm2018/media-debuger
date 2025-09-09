#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("FFprobe Viewer");

    ZWindowHelper::centerToCurrentScreen(this);
    // ZFfprobe probe;

    // qDebug() << "=== FFprobe Information ===";
    // qDebug() << "Version:" << probe.getVersion();
    // qDebug() << "\nBuild Configuration:" << probe.getBuildconf();
    // qDebug() << "\nFormats:" << probe.getFormats();
    // qDebug() << "\nMuxers:" << probe.getMuxers();
    // qDebug() << "\nDemuxers:" << probe.getDemuxers();
    // qDebug() << "\nDevices:" << probe.getDevices();
    // qDebug() << "\nCodecs:" << probe.getCodecs();
    // qDebug() << "\nDecoders:" << probe.getDecoders();
    // qDebug() << "\nEncoders:" << probe.getEncoders();
    // qDebug() << "\nBitstream Filters:" << probe.getBsfs();
    // qDebug() << "\nProtocols:" << probe.getProtocols();
    // qDebug() << "\nFilters:" << probe.getFilters();
    // qDebug() << "\nPixel Formats:" << probe.getPixfmts();
    // qDebug() << "\nChannel Layouts:" << probe.getLayouts();
    // qDebug() << "\nSample Formats:" << probe.getSamplefmts();
    // qDebug() << "\nColors:" << probe.getColors();
    // qDebug() << "===========================";

    InitConnectation();
}

MainWindow::~MainWindow()
{
    delete ui;

    for (auto it : m_basiclInfowindows) {
        if (it) {
            delete it;
        }
    }

    for (auto it : m_mediaInfowindows) {
        if (it) {
            delete it;
        }
    }
}

void MainWindow::InitConnectation()
{
    connect(ui->menuFile, &QMenu::triggered, this, &MainWindow::slotMenuFileTriggered);
    connect(ui->menuBasic_Info, &QMenu::triggered, this, &MainWindow::slotMenuBasic_InfoTriggered);
    connect(ui->menuMedia_Info, &QMenu::triggered, this, &MainWindow::slotMenuMedia_InfoTriggered);
    connect(ui->menuConfig, &QMenu::triggered, this, &MainWindow::slotMenuConfigTriggered);
}

void MainWindow::PopBasicInfoWindow(QString title, const QString &info, const QString& format_key)
{
    InfoWidgets *infoWindow = new InfoWidgets;
    infoWindow->setObjectName(title.replace(" ", "") + "Wg");
    m_basiclInfowindows.append(infoWindow);

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

    m_mediaInfowindows.append(mediaInfoWindow);
    mediaInfoWindow->setWindowTitle(title);
    mediaInfoWindow->show();
    ZWindowHelper::centerToParent(mediaInfoWindow);
    mediaInfoWindow->loadJson(info.toUtf8());

    qDebug() << title << info.size();
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
}

void MainWindow::slotMenuConfigTriggered(QAction *action)
{
    if (!action) {
        return;
    }

    GlobalConfingWG *configWg = new GlobalConfingWG;
    configWg->setWindowModality(Qt::ApplicationModal);
    configWg->setAttribute(Qt::WA_ShowModal, true);
    configWg->setCurrentTab(action->text());
    configWg->setAttribute(Qt::WA_DeleteOnClose);

    configWg->show();
}
