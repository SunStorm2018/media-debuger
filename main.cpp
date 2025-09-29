#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QDebug>

#include <common/common.h>
#include <common/zlogger.h>
#include <common/zffprobe.h>

/**
 * @brief logConfig
 * @param app
 *
 * Log configuration
 */
void logConfig(const QApplication& app);

/**
 * @brief commandConfig
 * @param app
 *
 * CommandLine configuration
 */
int commandConfig(const QApplication& app);

void translateConfig(const QApplication& app);

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

    QApplication app(argc, argv);

    translateConfig(app);

    return commandConfig(app);
}

void logConfig(const QApplication &app) {
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    ZLogger::instance()->loadConfig(settings);
    ZLogger::instance()->setConfigValue(LoggerConfig::DIRECTORY_KEY, "app_logs");
    ZLogger::instance()->setConfigValue(LoggerConfig::MAX_FILE_SIZE_KEY, 5);
    ZLogger::instance()->setConfigValue(LoggerConfig::MAX_FILES_KEY, 10);
    ZLogger::instance()->setConfigValue(LoggerConfig::LEVEL_KEY, static_cast<int>(LogLevel::LOG_INFO));

    QString fileName = "myapp_%1.log";
    ZLogger::instance()->setConfigValue(LoggerConfig::FILE_NAME_PATTERN_KEY, fileName);

    if (ZLogger::instance()->initializeWithConfig()) {
        qInfo() << "Logger initialized successfully with configuration";
    }

    ZLogger::instance()->saveConfig(settings);
    settings.sync();

    QObject::connect(&app, &QCoreApplication::aboutToQuit, [] {
        ZLogger::instance()->shutdown();
    });
}

int commandConfig(const QApplication& app) {
    QCommandLineParser parser;

    parser.setApplicationDescription("Media Debuger - Media file debugging tool");

    // options
    QCommandLineOption mediaInfoOption(QStringList() << "m" << "media-info",
                                       "Query media file audio and video stream information",
                                       "file");

    QCommandLineOption mediaInfoStreamTypeOption(QStringList() << "s" << "stream-type",
                                                 "Set available stream type (audio, video, a, v) ",
                                                 "type");

    QCommandLineOption mediaInfoFrameTypeOption(QStringList() << "f" << "frame-type",
                                                "Set available frame or packet type (frame, packet, f, p) ",
                                                "type");

    QCommandLineOption basicInfoOption(QStringList() << "b" << "basic-info",
                                       "Query basic info (version, buildconf, formats, muxers, demuxers, devices, codecs, "
                                       "decoders, encoders, bsfs, protocols, filters, pixfmts, layouts, samplefmts, colors, license)",
                                       "type");

    QCommandLineOption cliOption(QStringList() << "c" << "cli",
                                 "No graphical user interface is displayed; the system operates exclusively in command-line mode.");

    QCommandLineOption mainOption(QStringList() << "d" << "detail-window",
                                  "show mainwindow");

    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(basicInfoOption);
    parser.addOption(mediaInfoOption);
    parser.addOption(mediaInfoStreamTypeOption);
    parser.addOption(mediaInfoFrameTypeOption);
    parser.addOption(cliOption);
    parser.addOption(mainOption);

    parser.process(app);

    QString streamType, frameType, filePath, mediaCmd;

    // media info
    if (parser.isSet(mediaInfoOption)) {
        filePath = parser.value(mediaInfoOption);

        if (!QFileInfo(filePath).exists()) {
            qDebug() << "Error: Please specify the path of the media file" << filePath;
            return 1;
        }

        if (parser.isSet(mediaInfoStreamTypeOption)) {
            streamType = parser.value(mediaInfoStreamTypeOption);
        }

        if (parser.isSet(mediaInfoFrameTypeOption)) {
            frameType = parser.value(mediaInfoFrameTypeOption);
        }

        if (!QStringList{"audio", "video", "a", "v"}.contains(streamType)) {
            qDebug() << "Error: Please specify the frame type, (audio, video, a, v)";
            return 1;
        }
        streamType = streamType.left(1);

        if (!QStringList{"frame", "packet", "f", "p"}.contains(frameType)) {
            qDebug() << "Error: Please specify the frame type, (frame, packet, f, p)";
            return 1;
        }
        if (frameType.startsWith("f")) {
            frameType = SHOW_FRAMES;
        } else {
            frameType = SHOW_PACKETS;
        }

        mediaCmd = QString("%1 -select_streams %2:0").arg(frameType).arg(streamType);

        // cli options
        if (parser.isSet(cliOption)) {
            ZFfprobe ffprobe;

            QString mediaInfo = ffprobe.getMediaInfoJsonFormat(mediaCmd, filePath);
            if (mediaInfo.isEmpty()) {
                qDebug() << "Error: Media file information cannot be obtained. "
                            "Please check if the file path is correct";
                return 1;
            }

            printf("\nMedai Info:\n%s", mediaInfo.toUtf8().data());
            return 0;
        }
    }

    // basic info
    if (parser.isSet(basicInfoOption) && parser.isSet(cliOption)) {
        ZFfprobe ffprobe;

        QString function = parser.value(basicInfoOption).toLower();
        QString basicinfo;

        bool sucess = false;

        basicinfo = ffprobe.getBasicInfo(function, &sucess);

        if (!sucess) {
            qDebug() << "Error: Unsupported basic information type. The supported types include: \n" <<
                "version, buildconf, formats, muxers, demuxers, devices, codecs, decoders, encoders, bsfs, protocols, "
                "filters, pixfmts, layouts, samplefmts, colors, license";

            return 1;
        }

        printf("\nBasic Info:\n%s", basicinfo.toUtf8().data());
        return 0;
    }

    logConfig(app);

    // Get the singleton instance
    Common* common = Common::instance();

    // Initialize if not already initialized
    if (!common->isInitialized()) {
        common->initialize();
    }

    MainWindow w;

    if (parser.isSet(mediaInfoOption)) {
        QString filePath = parser.value(mediaInfoOption);

        w.showMediaInfo(filePath, mediaCmd, QString("%1").arg(mediaCmd), "table");
    }

    if (parser.isSet(basicInfoOption)) {
        QString function = parser.value(basicInfoOption);

        w.showBasicInfo(function, function, function);
    }

    if (parser.isSet(cliOption) || parser.isSet(mediaInfoOption) || parser.isSet(basicInfoOption)) {
        w.hide();
    } else {
        w.show();
    }

    return app.exec();

}

void translateConfig(const QApplication& app) {
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MediaDebuger_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }
}
