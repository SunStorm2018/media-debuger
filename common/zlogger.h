#ifndef ZLOGGER_H
#define ZLOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QMutex>
#include <QDateTime>
#include <QDir>
#include <QtGlobal>
#include <QSettings>

#include <common/common.h>

// 宏定义，方便使用
#define LOG_DEBUG(module, message)    ZLogger::instance()->debug(module, message)
#define LOG_INFO(module, message)     ZLogger::instance()->info(module, message)
#define LOG_WARNING(module, message)  ZLogger::instance()->warning(module, message)
#define LOG_ERROR(module, message)    ZLogger::instance()->error(module, message)
#define LOG_FATAL(module, message)    ZLogger::instance()->fatal(module, message)
/**
 * @brief 日志级别枚举
 */
enum LogLevel {
    LOG_DEBUG,   // 调试信息
    LOG_INFO,    // 一般信息
    LOG_WARNING, // 警告信息
    LOG_ERROR,   // 错误信息
    LOG_FATAL    // 严重错误
};

/**
 * @brief 高效的日志记录工具类
 * @details 支持多级别日志、文件滚动、线程安全，集成Qt消息处理器
 */
class ZLogger : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 获取日志实例（单例模式）
     * @return Logger实例指针
     */
    static ZLogger* instance();

    /**
     * @brief 初始化日志系统
     * @param logDir 日志目录
     * @param maxSize 单个日志文件最大大小（MB）
     * @param maxFiles 最大保留文件数
     * @param installMessageHandler 是否安装Qt消息处理器
     * @return 初始化是否成功
     */
    bool initialize(const QString& logDir,
                    quint64 maxSize = 10,
                    int maxFiles = 7,
                    bool installMessageHandler = true);

    /**
     * @brief 写入日志
     * @param level 日志级别
     * @param module 模块名称
     * @param message 日志消息
     */
    void write(LogLevel level, const QString& module, const QString& message);

    /**
     * @brief 处理Qt系统消息
     * @param type Qt消息类型
     * @param context 消息上下文
     * @param msg 消息内容
     */
    static void qtMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    /**
     * @brief 快捷方法：写入DEBUG级别日志
     */
    void debug(const QString& module, const QString& message);

    /**
     * @brief 快捷方法：写入INFO级别日志
     */
    void info(const QString& module, const QString& message);

    /**
     * @brief 快捷方法：写入WARNING级别日志
     */
    void warning(const QString& module, const QString& message);

    /**
     * @brief 快捷方法：写入ERROR级别日志
     */
    void error(const QString& module, const QString& message);

    /**
     * @brief 快捷方法：写入FATAL级别日志
     */
    void fatal(const QString& module, const QString& message);

    /**
     * @brief 设置最低日志级别
     * @param level 最低日志级别
     */
    void setMinLevel(LogLevel level);

    /**
     * @brief 设置是否捕获Qt系统消息
     * @param enable 是否启用
     */
    void setCaptureQtMessages(bool enable);

    /**
     * @brief 清理旧日志文件
     */
    void cleanupOldFiles();

    /**
     * @brief 关闭日志系统
     */
    void shutdown();

    /**
     * @brief 从配置文件中加载配置
     * @param settings QSettings实例
     */
    void loadConfig(QSettings& settings);

    /**
     * @brief 保存配置到配置文件
     * @param settings QSettings实例
     */
    void saveConfig(QSettings& settings);

    /**
     * @brief 使用当前配置初始化日志系统
     * @return 初始化是否成功
     */
    bool initializeWithConfig();

    /**
     * @brief 获取配置值
     */
    QVariant getConfigValue(const QString& key, const QVariant& defaultValue = QVariant());

    /**
     * @brief 设置配置值
     */
    void setConfigValue(const QString& key, const QVariant& value);

signals:
    /**
     * @brief 日志消息信号（可用于UI显示）
     */
    void logMessage(const QString& message);

private:
    explicit ZLogger(QObject *parent = nullptr);
    ~ZLogger();

    // 禁止拷贝和赋值
    ZLogger(const ZLogger&) = delete;
    ZLogger& operator=(const ZLogger&) = delete;

    /**
     * @brief 获取日志级别字符串
     */
    QString levelToString(LogLevel level) const;

    /**
     * @brief 获取当前日志文件名
     */
    QString getCurrentLogFileName() const;

    /**
     * @brief 滚动日志文件
     */
    bool rolloverLogFile();

    /**
     * @brief 检查是否需要滚动文件
     */
    bool needRollover() const;

    /**
     * @brief 将Qt消息类型转换为日志级别
     */
    LogLevel qtMsgTypeToLogLevel(QtMsgType type) const;

    /**
     * @brief 从文件路径中提取模块名
     */
    QString extractModuleFromPath(const char *file) const;

    static ZLogger* m_instance;           // 单例实例
    static QtMessageHandler m_oldHandler;// 原有的消息处理器

    QFile* m_logFile;                    // 日志文件
    QTextStream* m_textStream;           // 文本流
    QMutex m_mutex;                      // 互斥锁，保证线程安全
    QString m_logDir;                    // 日志目录
    quint64 m_maxFileSize;               // 最大文件大小（字节）
    int m_maxFiles;                      // 最大文件数量
    LogLevel m_minLevel;                 // 最低日志级别
    bool m_initialized;                  // 初始化标志
    bool m_captureQtMessages;            // 是否捕获Qt系统消息

    QMap<QString, QVariant> m_config; // 存储配置的容器
};



#endif // ZLOGGER_H
