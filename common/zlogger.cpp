#include "zlogger.h"
#include <QCoreApplication>
#include <QDebug>
#include <QRegularExpression>

// 初始化静态成员
ZLogger* ZLogger::m_instance = nullptr;
QtMessageHandler ZLogger::m_oldHandler = nullptr;

ZLogger::ZLogger(QObject *parent)
    : QObject(parent)
    , m_logFile(nullptr)
    , m_textStream(nullptr)
    , m_maxFileSize(10 * 1024 * 1024) // 默认10MB
    , m_maxFiles(7)                   // 默认保留7个文件
    , m_minLevel(LogLevel::LOG_DEBUG)     // 默认记录所有级别
    , m_initialized(false)
    , m_captureQtMessages(true)
{
    // 初始化默认配置
    m_config[LoggerConfig::ENABLED_KEY] = LoggerConfig::DEFAULT_ENABLED;
    m_config[LoggerConfig::LEVEL_KEY] = LoggerConfig::DEFAULT_LOG_LEVEL;
    m_config[LoggerConfig::DIRECTORY_KEY] = LoggerConfig::DEFAULT_DIRECTORY;
    m_config[LoggerConfig::MAX_FILE_SIZE_KEY] = LoggerConfig::DEFAULT_MAX_FILE_SIZE;
    m_config[LoggerConfig::MAX_FILES_KEY] = LoggerConfig::DEFAULT_MAX_FILES;
    m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY] = LoggerConfig::DEFAULT_CAPTURE_QT_MESSAGES;
    m_config[LoggerConfig::FILE_NAME_PATTERN_KEY] = LoggerConfig::DEFAULT_FILE_NAME_PATTERN;
    m_config[LoggerConfig::LOG_FORMAT_KEY] = LoggerConfig::DEFAULT_LOG_FORMAT;
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
    m_maxFileSize = maxSize * 1024 * 1024; // 转换为字节
    m_maxFiles = maxFiles;
    m_captureQtMessages = installMessageHandler;

    // 创建日志目录
    QDir dir(m_logDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "Failed to create log directory:" << m_logDir;
            return false;
        }
    }

    // 打开日志文件
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

    // 安装Qt消息处理器
    if (m_captureQtMessages) {
        m_oldHandler = qInstallMessageHandler(qtMessageHandler);
    }

    // 清理旧文件
    cleanupOldFiles();

    return true;
}

void ZLogger::write(LogLevel level, const QString& module, const QString& message)
{
    if (level < m_minLevel || !m_initialized) {
        return;
    }

    QMutexLocker locker(&m_mutex);

    // 检查是否需要滚动文件
    if (needRollover()) {
        if (!rolloverLogFile()) {
            qWarning() << "Failed to rollover log file";
            return;
        }
    }

    // 格式化日志消息
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString levelStr = levelToString(level);
    QString logMessage = QString("[%1] [%2] [%3] %4\n")
                             .arg(timestamp)
                             .arg(levelStr)
                             .arg(module)
                             .arg(message);

    // 写入文件
    *m_textStream << logMessage;
    m_textStream->flush();

// 同时输出到控制台（在调试模式下）
#ifdef QT_DEBUG
    qDebug().noquote() << logMessage.trimmed();
#endif

    // 发送信号（可用于UI显示）
    // emit logMessage(logMessage);
}

void ZLogger::qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    ZLogger* logger = instance();

    if (!logger->m_captureQtMessages) {
        // 如果禁用捕获，调用原有的处理器
        if (m_oldHandler) {
            m_oldHandler(type, context, msg);
        }
        return;
    }

    // 转换消息级别
    LogLevel level = logger->qtMsgTypeToLogLevel(type);

    // 提取模块名（从文件路径中获取）
    QString module = logger->extractModuleFromPath(context.file);

    // 格式化Qt消息
    QString formattedMsg;
    if (context.file && context.line && context.function) {
        formattedMsg = QString("%1 (File: %2, Line: %3, Function: %4)")
        .arg(msg)
            .arg(context.file)
            .arg(context.line)
            .arg(context.function);
    } else {
        formattedMsg = msg;
    }

    // 写入日志
    logger->write(level, module, formattedMsg);

    // 调用原有的消息处理器（输出到控制台）
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

    // 恢复原有的消息处理器
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

    settings.beginGroup(LoggerConfig::LOG_GROUP);

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

    settings.endGroup();

    // 更新日志级别
    int level = m_config[LoggerConfig::LEVEL_KEY].toInt();
    setMinLevel(static_cast<LogLevel>(level));

    // 更新Qt消息捕获设置
    m_captureQtMessages = m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY].toBool();
}

void ZLogger::saveConfig(QSettings& settings)
{
    QMutexLocker locker(&m_mutex);

    settings.beginGroup(LoggerConfig::LOG_GROUP);

    settings.setValue(LoggerConfig::ENABLED_KEY, m_config[LoggerConfig::ENABLED_KEY]);
    settings.setValue(LoggerConfig::LEVEL_KEY, m_config[LoggerConfig::LEVEL_KEY]);
    settings.setValue(LoggerConfig::DIRECTORY_KEY, m_config[LoggerConfig::DIRECTORY_KEY]);
    settings.setValue(LoggerConfig::MAX_FILE_SIZE_KEY, m_config[LoggerConfig::MAX_FILE_SIZE_KEY]);
    settings.setValue(LoggerConfig::MAX_FILES_KEY, m_config[LoggerConfig::MAX_FILES_KEY]);
    settings.setValue(LoggerConfig::CAPTURE_QT_MESSAGES_KEY, m_config[LoggerConfig::CAPTURE_QT_MESSAGES_KEY]);
    settings.setValue(LoggerConfig::FILE_NAME_PATTERN_KEY, m_config[LoggerConfig::FILE_NAME_PATTERN_KEY]);
    settings.setValue(LoggerConfig::LOG_FORMAT_KEY, m_config[LoggerConfig::LOG_FORMAT_KEY]);

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

    // 设置日志格式
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

    // 特殊处理某些配置项的实时更新
    if (key == LoggerConfig::LEVEL_KEY) {
        int level = value.toInt();
        setMinLevel(static_cast<LogLevel>(level));
    }
    else if (key == LoggerConfig::CAPTURE_QT_MESSAGES_KEY) {
        m_captureQtMessages = value.toBool();
    }
}

QString ZLogger::levelToString(LogLevel level) const
{
    switch (level) {
    case LogLevel::LOG_DEBUG:   return "DEBUG";
    case LogLevel::LOG_INFO:    return "INFO";
    case LogLevel::LOG_WARNING: return "WARN";
    case LogLevel::LOG_ERROR:   return "ERROR";
    case LogLevel::LOG_FATAL:   return "FATAL";
    default:                return "UNKNOWN";
    }
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

    // 检查文件大小是否超过限制
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

    // 重命名当前文件（添加时间戳）
    QString oldFileName = getCurrentLogFileName();
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString newFileName = QDir(m_logDir).filePath(QString("app_%1_%2.log").arg(QDateTime::currentDateTime().toString("yyyyMMdd")).arg(timestamp));

    QFile::rename(oldFileName, newFileName);

    // 创建新文件
    m_logFile = new QFile(oldFileName);
    if (!m_logFile->open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        delete m_logFile;
        m_logFile = nullptr;
        return false;
    }

    m_textStream = new QTextStream(m_logFile);
    m_textStream->setCodec("UTF-8");

    // 清理旧文件
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

    // 从文件路径中提取文件名（不含扩展名）
    QFileInfo fileInfo(filePath);
    QString fileName = fileInfo.baseName();

    // 如果文件名太长，截断
    if (fileName.length() > 20) {
        fileName = fileName.left(20) + "...";
    }

    return fileName.isEmpty() ? "Unknown" : fileName;
}

/**
#include "Logger.h"
#include <QCoreApplication>
#include <QDebug>

void testFunction()
{
    qDebug() << "This is a debug message from Qt";
    qInfo() << "This is an info message from Qt";
    qWarning() << "This is a warning message from Qt";
    qCritical() << "This is a critical message from Qt";
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 初始化日志系统（自动安装Qt消息处理器）
    ZLogger::instance()->initialize("logs", 5, 10);

    // 设置最低日志级别
    ZLogger::instance()->setMinLevel(LogLevel::INFO);

    // 使用宏记录自定义日志
    LOG_INFO("Main", "Application started");
    LOG_DEBUG("Main", "This debug message won't be logged due to level setting");

    // 测试Qt系统消息捕获
    testFunction();

    // 也可以手动记录日志
    Logger::instance()->warning("Network", "Connection timeout");
    Logger::instance()->error("Database", "Failed to connect to database");

    // 临时禁用Qt消息捕获
    Logger::instance()->setCaptureQtMessages(false);
    qDebug() << "This message will only go to console";
    Logger::instance()->setCaptureQtMessages(true);

    // 程序退出时自动清理
    QObject::connect(&a, &QCoreApplication::aboutToQuit, [] {
        Logger::instance()->shutdown();
    });

    return a.exec();
}
*/
