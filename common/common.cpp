#include "common.h"

QList<QStringList> Common::logLevels = {
    {"quiet", "-8", "Show nothing at all; be silent."},
    {"panic", "0", "Only show fatal errors which could lead the process to crash, such as an assertion failure. This is not currently used for anything."},
    {"fatal", "8", "Only show fatal errors. These are errors after which the process absolutely cannot continue."},
    {"error", "16", "Show all errors, including ones which can be recovered from."},
    {"warning", "24", "Show all warnings and errors. Any message related to possibly incorrect or unexpected events will be shown."},
    {"info", "32", "Show informative messages during processing. This is in addition to warnings and errors. This is the default value."},
    {"verbose", "40", "Same as info, except more verbose."},
    {"debug", "48", "Show everything, including debugging information."},
    {"trace", "56", "shows extremely detailed, low-level tracing information about the internal execution flow of the code, allowing for deep technical debugging and analysis."}
};

const QStringList CONFIG_GROUPS = {
    LOG_GROUP,
    GENERAL_GROUP,
    MAINWINDOW_GROUP,
    WINDOWHEAD_GROUP,
    RECENTFILES_GROUP
};

QMutex Common::m_mutex;
Common* Common::m_instance = nullptr;

Common::Common()
    : m_initialized(false)
{
    // Private constructor
}

Common::~Common()
{
    // Cleanup resources
    qDebug() << "Common singleton destroyed";
}

Common* Common::instance()
{
    QMutexLocker locker(&m_mutex);
    if (!m_instance) {
        m_instance = new Common();
    }
    return m_instance;
}

void Common::destroy()
{
    QMutexLocker locker(&m_mutex);
    delete m_instance;
    m_instance = nullptr;
}

void Common::setConfigValue(const QString& key, const QVariant& value)
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    settings.setValue(key, value);
}

QVariant Common::getConfigValue(const QString& key, const QVariant& defaultValue) const
{
    QSettings settings(ORGANIZATION_NAME, APPLICATION_NAME);
    return settings.value(key, defaultValue);
}

void Common::initialize()
{
    if (m_initialized) {
        return;
    }
    m_initialized = true;
}

bool Common::isInitialized() const
{
    return m_initialized;
}


