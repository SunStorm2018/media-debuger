#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QDebug>
#include <QCoreApplication>

#define CURRENTFILE "currentFile"
#define CURRENTFILES "currentFiles"
#define CURRENTDir "currentDir"

#define ORGANIZATION_NAME "MediaTools"
#define APPLICATION_NAME "DebuggerPro"
#define APPLICATION_VERSION "1.0.0"

constexpr auto LOG_GROUP = "LogSettings";
constexpr auto GENERAL_GROUP = "GeneralSettings";
constexpr auto MAINWINDOW_GROUP = "MainWindowSettings";
constexpr auto WINDOWHEAD_GROUP = "WindowHeadSettings";

extern const QStringList CONFIG_GROUPS;

// config
/**
 * @brief Macro definitions and default values for log configuration
 */
namespace LoggerConfig {
// Configuration key names
constexpr auto ENABLED_KEY = "enabled";
constexpr auto LEVEL_KEY = "logLevel";
constexpr auto DIRECTORY_KEY = "logDirectory";
constexpr auto MAX_FILE_SIZE_KEY = "maxFileSizeMB";
constexpr auto MAX_FILES_KEY = "maxFiles";
constexpr auto CAPTURE_QT_MESSAGES_KEY = "captureQtMessages";
constexpr auto FILE_NAME_PATTERN_KEY = "fileNamePattern";
constexpr auto LOG_FORMAT_KEY = "logFormat";

// Default values
constexpr bool DEFAULT_ENABLED = true;
constexpr int DEFAULT_LOG_LEVEL = 1; // INFO level
constexpr auto DEFAULT_DIRECTORY = "logs";
constexpr int DEFAULT_MAX_FILE_SIZE = 10; // MB
constexpr int DEFAULT_MAX_FILES = 7;
constexpr bool DEFAULT_CAPTURE_QT_MESSAGES = true;
constexpr auto DEFAULT_FILE_NAME_PATTERN = "app_%1.log"; // %1 will be replaced by date
constexpr auto DEFAULT_LOG_FORMAT = "[%{time yyyy-MM-dd hh:mm:ss.zzz}] [%{type}] [%{file}:%{line}] %{message}";

/**
 * @brief String representation of log level enum
 */
static QString logLevelToString(int level) {
    switch (level) {
    case 0: return "DEBUG";
    case 1: return "INFO";
    case 2: return "WARNING";
    case 3: return "ERROR";
    case 4: return "FATAL";
    default: return "INFO";
    }
}

/**
 * @brief Log level enum value from string
 */
static int stringToLogLevel(const QString& levelStr) {
    if (levelStr == "DEBUG") return 0;
    if (levelStr == "INFO") return 1;
    if (levelStr == "WARNING") return 2;
    if (levelStr == "ERROR") return 3;
    if (levelStr == "FATAL") return 4;
    return 1; // Default to INFO
}
} // namespace LoggerConfig

class Common
{
    // Q_OBJECT
public:
    Common(const Common&) = delete;
    Common& operator=(const Common&) = delete;

    static Common* instance();
    static void destroy();

    void setConfigValue(const QString& key, const QVariant &value);
    QVariant getConfigValue(const QString& key, const QVariant &defaultValue = QString()) const;

    void initialize();
    bool isInitialized() const;

    static QList<QStringList> logLevels;

private:
    Common();
    ~Common();

    bool m_initialized;
    static QMutex m_mutex;
    static Common* m_instance;
};

#endif // COMMON_H
