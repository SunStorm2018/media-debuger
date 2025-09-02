#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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

    for (auto it : m_infowindows) {
        if (it) {
            delete it;
        }
    }
}

void MainWindow::InitConnectation()
{
    connect(ui->menuBasic_Info, &QMenu::triggered, [=](QAction *action){
        if (!action) {
            return;
        }

        QString function = action->objectName().replace("action", "get");
        QString retVal;
        QMetaObject::invokeMethod(&m_probe, function.toUtf8(), Qt::DirectConnection,
                                  Q_RETURN_ARG(QString, retVal));

        PopInfoWindow(action->objectName().replace("action", "Detail Info : "), retVal, action->objectName().replace("action", ""));
    });
}

void MainWindow::PopInfoWindow(QString title, const QString &info, const QString& format_key)
{
    InfoWidgets *infoWindow = new InfoWidgets;
    m_infowindows.append(infoWindow);

    infoWindow->setWindowTitle(title);
    infoWindow->show();
    ZWindowHelper::centerToParent(infoWindow);
    infoWindow->init_detail_tb(info, format_key.toLower());

    qDebug() << title << info;

}
