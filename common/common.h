#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QSet>
#include <QDebug>
#include <QMimeData>
#include <QCoreApplication>
#include <QMimeDatabase>
#include <QUrl>
#include <QFileInfo>

#define CURRENTFILE "currentFile"
#define CURRENTFILES "currentFiles"
#define CURRENTDir "currentDir"

#define ORGANIZATION_NAME "MediaTools"
#define APPLICATION_NAME "DebuggerPro"
#define APPLICATION_VERSION "1.0.0"

constexpr auto LOG_SETTINGS_GROUP = "Log";
constexpr auto GENERAL_SETTINGS_GROUP = "General";
constexpr auto MAINWINDOW_SETTINGS_GROUP = "MainWindow";
constexpr auto WINDOWHEAD_SETTINGS_GROUP = "WindowHead";
constexpr auto RECENTFILES_SETTINGS_GROUP = "RecentFiles";

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

    // Check if the file is a supported video format
    static bool isSupportedVideoFile(const QString &filePath);

    // Check if the file is a video (generic method)
    static bool isVideoFile(const QString &filePath);

    // Check if the file is an audio file
    static bool isAudioFile(const QString &filePath);

    // Check if the file is a media file (video or audio)
    static bool isMediaFile(const QString &filePath);

    // Get all supported video MIME types
    static QSet<QString> supportedVideoMimeTypes();

    // Get all supported video file extensions
    static QSet<QString> supportedVideoExtensions();

    // Extract supported media file paths from MIME data
    static QStringList extractSupportedMediaFiles(const QMimeData *mimeData);

    // Check if MIME data contains supported media files
    static bool containsSupportedMediaFiles(const QMimeData *mimeData);

private:
    Common();
    ~Common();

    bool m_initialized;
    static QMutex m_mutex;
    static Common* m_instance;

    // Initialize supported MIME types (lazy initialization)
    static const QSet<QString> &getSupportedVideoMimeTypes();

    // Initialize supported file extensions (lazy initialization)
    static const QSet<QString> &getSupportedVideoExtensions();
};

#endif // COMMON_H
