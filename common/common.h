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
