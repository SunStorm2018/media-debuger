#include "zlogger.h"
#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>

// Initialize static members
ZLogger* ZLogger::m_instance = nullptr;
QtMessageHandler ZLogger::m_oldHandler = nullptr;

ZLogger::ZLogger(QObject *parent)
    : QObject(parent)
    , m_logFile(nullptr)
    , m_textStream(nullptr)
    , m_maxFileSize(10 * 1024 * 1024) // Default 10MB
    , m_maxFiles(7)                   // Default keep 7 files
    , m_minLevel(LogLevel::LOG_DEBUG)     // Default log all levels
    , m_initialized(false)
    , m_captureQtMessages(true)
{
    // Initialize default configuration
    m_config[LoggerConfig::ENABLED_KEY] = LoggerConfig::DEFAULT_ENABLED;
    m_config[LoggerConfig::LEVEL_KEY] = LoggerConfig::DEFAULT_LOG_LEVEL;
    m_config[LoggerConfig::DIRECTORY_KEY] = LoggerConfig::DEFAULT_DIRECTORY;
    m_config[LoggerConfig::MAX_FILE_SIZE_KEY] = LoggerConfig::DEFAULT_MAX_FILE_SIZE;
    m_config[LoggerConfig::MAX_FILES_KEY] = LoggerConfig::DEFAULT_MAX_FILES;
    m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY] = LoggerConfig::DEFAULT_CAPTURE_QT_MESSAGES;
    m_config[LoggerConfig::FILE_NAME_PATTERN_KEY] = LoggerConfig::DEFAULT_FILE_NAME_PATTERN;
    m_config[LoggerConfig::LOG_FORMAT_KEY] = LoggerConfig::DEFAULT_LOG_FORMAT;
    m_config[LoggerConfig::ENABLED_FILE] = true;
    m_config[LoggerConfig::ENABLED_LINE] = true;
    m_config[LoggerConfig::ENABLED_FUNCTION] = true;
}

ZLogger::~ZLogger()
{
    shutdown();
}

ZLogger* ZLogger::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (!m_instance) {
        m_instance = new ZLogger();
    }
    return m_instance;
}

bool ZLogger::initialize(const QString& logDir, quint64 maxSize, int maxFiles, bool installMessageHandler)
{
    // QMutexLocker locker(&m_mutex);

    if (m_initialized) {
        return true;
    }

    m_logDir = logDir;
    m_maxFileSize = maxSize * 1024 * 1024; // Convert to bytes
    m_maxFiles = maxFiles;
    m_captureQtMessages = installMessageHandler;

    // Create log directory
    QDir dir(m_logDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create log directory:" << m_logDir;
            return false;
        }
    }

    // Open log file
    QString fileName = getCurrentLogFileName();
    m_logFile = new QFile(fileName);

    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open log file:" << fileName;
        delete m_logFile;
        m_logFile = nullptr;
        return false;
    }

    m_textStream = new QTextStream(m_logFile);
    m_textStream->setCodec("UTF-8");
    m_initialized = true;

    // Install Qt message handler
    if (m_captureQtMessages) {
        m_oldHandler = qInstallMessageHandler(qtMessageHandler);
    }

    // Clean up old files
    cleanupOldFiles();

    return true;
}

void ZLogger::write(LogLevel level, const QString& module, const QString& message)
{

    QMutexLocker locker(&m_mutex);

    // Check if file needs to be rolled over
    if (needRollover()) {
        if (!rolloverLogFile()) {
            qWarning() << "Failed to rollover log file";
            return;
        }
    }

    // Format log message
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString logMsg = QString("[%1] [%2] [%3] %4")
                         .arg(timestamp)
                         .arg(levelStr)
                         .arg(module)
                         .arg(message);

    emit logMessage(logMsg);

    if (level < m_minLevel || !m_initialized) {
        return;
    }

    // Write to file
    *m_textStream << logMsg << "\n";
    m_textStream->flush();

// Also output to console in debug mode
#ifdef QT_DEBUG
    qDebug().noquote() << logMsg.trimmed();
#endif
}

void ZLogger::qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    ZLogger* logger = instance();

    if (!logger->m_captureQtMessages) {
        // If capture is disabled, call original handler
        if (m_oldHandler) {
            m_oldHandler(type, context, msg);
        }
        return;
    }

    // Convert message level
    LogLevel level = logger->qtMsgTypeToLogLevel(type);

    // Extract module name from file path
    QString module = logger->extractModuleFromPath(context.file);

    // Format Qt message
    QString formattedMsg;
    if (context.file && context.line && context.function) {
        formattedMsg = QString("%1%2%3%4")
        .arg(msg)
            .arg(logger->getConfigValue(LoggerConfig::ENABLED_FILE).toBool() ? QString(" [File] ") + context.file : "")
            .arg(logger->getConfigValue(LoggerConfig::ENABLED_LINE).toBool() ? QString(" [Line] ") + QString::number(context.line) : "")
            .arg(logger->getConfigValue(LoggerConfig::ENABLED_FUNCTION).toBool() ? QString(" [Fun] ") + context.function : "");
    } else {
        formattedMsg = msg;
    }

    // Write log
    logger->write(level, module, formattedMsg);

    // Call original message handler to output to console
    if (m_oldHandler) {
        m_oldHandler(type, context, msg);
    }
}

void ZLogger::debug(const QString& module, const QString& message)
{
    write(LogLevel::LOG_DEBUG, module, message);
}

void ZLogger::info(const QString& module, const QString& message)
{
    write(LogLevel::LOG_INFO, module, message);
}

void ZLogger::warning(const QString& module, const QString& message)
{
    write(LogLevel::LOG_WARNING, module, message);
}

void ZLogger::error(const QString& module, const QString& message)
{
    write(LogLevel::LOG_ERROR, module, message);
}

void ZLogger::fatal(const QString& module, const QString& message)
{
    write(LogLevel::LOG_FATAL, module, message);
}

void ZLogger::setMinLevel(LogLevel level)
{
    // QMutexLocker locker(&m_mutex);
    m_minLevel = level;
}

void ZLogger::setCaptureQtMessages(bool enable)
{
    QMutexLocker locker(&m_mutex);
    m_captureQtMessages = enable;
}

void ZLogger::cleanupOldFiles()
{
    QDir dir(m_logDir);
    QStringList filters;
    filters << "app_*.log";

    QFileInfoList files = dir.entryInfoList(filters, QDir::Files, QDir::Time | QDir::Reversed);

    while (files.size() > m_maxFiles) {
        QFileInfo fileInfo = files.takeFirst();
        QFile::remove(fileInfo.absoluteFilePath());
    }
}

void ZLogger::shutdown()
{
    QMutexLocker locker(&m_mutex);

    // Restore original message handler
    if (m_captureQtMessages && m_oldHandler) {
        qInstallMessageHandler(m_oldHandler);
        m_oldHandler = nullptr;
    }

    if (m_textStream) {
        m_textStream->flush();
        delete m_textStream;
        m_textStream = nullptr;
    }

    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }

    m_initialized = false;
}

void ZLogger::loadConfig(QSettings &settings)
{
    QMutexLocker locker(&m_mutex);

    settings.beginGroup(LOG_SETTINGS_GROUP);

    m_config[LoggerConfig::ENABLED_KEY] = settings.value(
        LoggerConfig::ENABLED_KEY,
        m_config[LoggerConfig::ENABLED_KEY]
        );

    m_config[LoggerConfig::LEVEL_KEY] = settings.value(
        LoggerConfig::LEVEL_KEY,
        m_config[LoggerConfig::LEVEL_KEY]
        );

    m_config[LoggerConfig::DIRECTORY_KEY] = settings.value(
        LoggerConfig::DIRECTORY_KEY,
        m_config[LoggerConfig::DIRECTORY_KEY]
        );

    m_config[LoggerConfig::MAX_FILE_SIZE_KEY] = settings.value(
        LoggerConfig::MAX_FILE_SIZE_KEY,
        m_config[LoggerConfig::MAX_FILE_SIZE_KEY]
        );

    m_config[LoggerConfig::MAX_FILES_KEY] = settings.value(
        LoggerConfig::MAX_FILES_KEY,
        m_config[LoggerConfig::MAX_FILES_KEY]
        );

    m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY] = settings.value(
        LoggerConfig::CAPTURE_QT_MESSAGES_KEY,
        m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY]
        );

    m_config[LoggerConfig::FILE_NAME_PATTERN_KEY] = settings.value(
        LoggerConfig::FILE_NAME_PATTERN_KEY,
        m_config[LoggerConfig::FILE_NAME_PATTERN_KEY]
        );

    m_config[LoggerConfig::LOG_FORMAT_KEY] = settings.value(
        LoggerConfig::LOG_FORMAT_KEY,
        m_config[LoggerConfig::LOG_FORMAT_KEY]
        );

    m_config[LoggerConfig::ENABLED_FILE] = settings.value(
        LoggerConfig::ENABLED_FILE,
        m_config[LoggerConfig::ENABLED_FILE]
        );

    m_config[LoggerConfig::ENABLED_LINE] = settings.value(
        LoggerConfig::ENABLED_LINE,
        m_config[LoggerConfig::ENABLED_LINE]
        );

    m_config[LoggerConfig::ENABLED_FUNCTION] = settings.value(
        LoggerConfig::ENABLED_FUNCTION,
        m_config[LoggerConfig::ENABLED_FUNCTION]
        );

    settings.endGroup();

    // Update log level
    int level = m_config[LoggerConfig::LEVEL_KEY].toInt();
    setMinLevel(static_cast<LogLevel>(level));

    // Update Qt message capture setting
    m_captureQtMessages = m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY].toBool();
}

void ZLogger::saveConfig(QSettings& settings)
{
    QMutexLocker locker(&m_mutex);

    settings.beginGroup(LOG_SETTINGS_GROUP);

    settings.setValue(LoggerConfig::ENABLED_KEY, m_config[LoggerConfig::ENABLED_KEY]);
    settings.setValue(LoggerConfig::LEVEL_KEY, m_config[LoggerConfig::LEVEL_KEY]);
    settings.setValue(LoggerConfig::DIRECTORY_KEY, m_config[LoggerConfig::DIRECTORY_KEY]);
    settings.setValue(LoggerConfig::MAX_FILE_SIZE_KEY, m_config[LoggerConfig::MAX_FILE_SIZE_KEY]);
    settings.setValue(LoggerConfig::MAX_FILES_KEY, m_config[LoggerConfig::MAX_FILES_KEY]);
    settings.setValue(LoggerConfig::CAPTURE_QT_MESSAGES_KEY, m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY]);
    settings.setValue(LoggerConfig::FILE_NAME_PATTERN_KEY, m_config[LoggerConfig::FILE_NAME_PATTERN_KEY]);
    settings.setValue(LoggerConfig::LOG_FORMAT_KEY, m_config[LoggerConfig::LOG_FORMAT_KEY]);
    settings.setValue(LoggerConfig::ENABLED_FILE, m_config[LoggerConfig::ENABLED_FILE]);
    settings.setValue(LoggerConfig::ENABLED_LINE, m_config[LoggerConfig::ENABLED_LINE]);
    settings.setValue(LoggerConfig::ENABLED_FUNCTION, m_config[LoggerConfig::ENABLED_FUNCTION]);

    settings.endGroup();
}

bool ZLogger::initializeWithConfig()
{
    QMutexLocker locker(&m_mutex);

    if (!m_config[LoggerConfig::ENABLED_KEY].toBool()) {
        qInfo() << "Logging is disabled in configuration";
        return false;
    }

    QString logDir = m_config[LoggerConfig::DIRECTORY_KEY].toString();
    quint64 maxSize = m_config[LoggerConfig::MAX_FILE_SIZE_KEY].toUInt();
    int maxFiles = m_config[LoggerConfig::MAX_FILES_KEY].toInt();
    bool captureQtMsg = m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY].toBool();

    // Set log format
    QString logFormat = m_config[LoggerConfig::LOG_FORMAT_KEY].toString();
    qSetMessagePattern(logFormat);

    return initialize(logDir, maxSize, maxFiles, captureQtMsg);
}

QVariant ZLogger::getConfigValue(const QString& key, const QVariant& defaultValue)
{
    QMutexLocker locker(&m_mutex);
    return m_config.value(key, defaultValue);
}

void ZLogger::setConfigValue(const QString& key, const QVariant& value)
{
    QMutexLocker locker(&m_mutex);
    m_config[key] = value;

    // Special handling for real-time updates of certain config items
    if (key == LoggerConfig::LEVEL_KEY) {
        int level = value.toInt();
        setMinLevel(static_cast<LogLevel>(level));
    }
    else if (key == LoggerConfig::CAPTURE_QT_MESSAGES_KEY) {
        m_captureQtMessages = value.toBool();
    }
}

QString ZLogger::levelToString(const LogLevel &level) const
{
    switch (level) {
    case LogLevel::LOG_DEBUG:   return "DEBUG";
    case LogLevel::LOG_INFO:    return "INFO";
    case LogLevel::LOG_WARNING: return "WARN";
    case LogLevel::LOG_ERROR:   return "ERROR";
    case LogLevel::LOG_FATAL:   return "FATAL";
    default:                    return "UNKNOWN";
    }
}

LogLevel ZLogger::stringToLevel(const QString &level) const
{
    if (level == "DEBUG")       return LOG_DEBUG;
    if (level == "INFO")        return LOG_INFO;
    if (level == "WARNING")     return LOG_WARNING;
    if (level == "ERROR")       return LOG_ERROR;
    if (level == "FATAL")       return LOG_FATAL;
    return LOG_INFO;            // Default to INFO
}

QString ZLogger::getCurrentLogFileName() const
{
    QString dateStr = QDateTime::currentDateTime().toString("yyyyMMdd");
    return QDir(m_logDir).filePath(QString("app_%1.log").arg(dateStr));
}

bool ZLogger::needRollover() const
{
    if (!m_logFile) {
        return false;
    }

    // Check if file size exceeds limit
    return m_logFile->size() >= m_maxFileSize;
}

bool ZLogger::rolloverLogFile()
{
    if (m_textStream) {
        m_textStream->flush();
        delete m_textStream;
        m_textStream = nullptr;
    }

    if (m_logFile) {
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }

    // Rename current file with timestamp
    QString oldFileName = getCurrentLogFileName();
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString newFileName = QDir(m_logDir).filePath(QString("app_%1_%2.log").arg(QDateTime::currentDateTime().toString("yyyyMMdd")).arg(timestamp));

    QFile::rename(oldFileName, newFileName);

    // Create new file
    m_logFile = new QFile(oldFileName);
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        delete m_logFile;
        m_logFile = nullptr;
        return false;
    }

    m_textStream = new QTextStream(m_logFile);
    m_textStream->setCodec("UTF-8");

    // Clean up old files
    cleanupOldFiles();

    return true;
}

LogLevel ZLogger::qtMsgTypeToLogLevel(QtMsgType type) const
{
    switch (type) {
    case QtDebugMsg:    return LogLevel::LOG_DEBUG;
    case QtInfoMsg:     return LogLevel::LOG_INFO;
    case QtWarningMsg:  return LogLevel::LOG_WARNING;
    case QtCriticalMsg: return LogLevel::LOG_ERROR;
    case QtFatalMsg:    return LogLevel::LOG_FATAL;
    default:            return LogLevel::LOG_INFO;
    }
}

QString ZLogger::extractModuleFromPath(const char *file) const
{
    if (!file) {
        return "Unknown";
    }

    QString filePath = QString::fromUtf8(file);
    
    int colonIndex = filePath.lastIndexOf(':');
    if (colonIndex != -1) {
        QString afterColon = filePath.mid(colonIndex + 1);
        bool isLineNumber = false;
        afterColon.toInt(&isLineNumber);
        if (isLineNumber) {
            filePath = filePath.left(colonIndex);
        }
    }

    // Extract filename (without extension) from file path
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.baseName();

    // If filename is too long, truncate
    if (fileName.length() > 20) {
        fileName = fileName.left(20) + "...";
    }

    return fileName.isEmpty() ? "Unknown" : fileName;
}

/**
#include "Logger.h"
#include <QCoreApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Use macro to record custom log
    LOG_INFO("Main", "Application started");
    LOG_DEBUG("Main", "This debug message won't be logged due to level setting");

    // Test Qt system message capture
    qDebug() << "This is a debug message from Qt";
    qInfo() << "This is an info message from Qt";
    qWarning() << "This is a warning message from Qt";
    qCritical() << "This is a critical message from Qt";

    // Can also manually record logs
    ZLogger::instance()->warning("Network", "Connection timeout");
    ZLogger::instance()->error("Database", "Failed to connect to database");
    // Temporarily disable Qt message capture
    ZLogger::instance()->setCaptureQtMessages(false);
    qDebug() << "This message will only go to console";
    ZLogger::instance()->setCaptureQtMessages(true);

    // Automatic cleanup on program exit
    QObject::connect(&a, &QCoreApplication::aboutToQuit, [] {
        ZLogger::instance()->shutdown();
    });

    return a.exec();
}
*/
