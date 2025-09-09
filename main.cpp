#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

#include <common/common.h>
#include <common/zlogger.h>

/**
 * @brief loggConfig
 * 日志配置
 */
void loggConfig(const QApplication& app);

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    QCoreApplication::setApplicationVersion(APPLICATION_VERSION);

    QApplication app(argc, argv);

    loggConfig(app);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MediaDebuger_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    // Get the singleton instance
    Common* common = Common::instance();

    // Initialize if not already initialized
    if (!common->isInitialized()) {
        common->initialize();
    }

    MainWindow w;
    w.show();
    return app.exec();
}

void loggConfig(const QApplication &app) {
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
