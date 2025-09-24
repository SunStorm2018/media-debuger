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
    LOG_SETTINGS_GROUP,
    GENERAL_SETTINGS_GROUP,
    MAINWINDOW_SETTINGS_GROUP,
    WINDOWHEAD_SETTINGS_GROUP,
    RECENTFILES_SETTINGS_GROUP
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

bool Common::isSupportedVideoFile(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.isFile() || !fileInfo.exists()) {
        return false;
    }

    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(filePath, QMimeDatabase::MatchExtension);

    const QSet<QString> &supportedMimeTypes = getSupportedVideoMimeTypes();
    if (supportedMimeTypes.contains(mimeType.name())) {
        return true;
    }

    const QSet<QString> &supportedExtensions = getSupportedVideoExtensions();
    QString extension = fileInfo.suffix().toLower();
    return supportedExtensions.contains(extension);
}

bool Common::isVideoFile(const QString &filePath)
{
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

    return mimeType.name().startsWith("video/");
}

bool Common::isAudioFile(const QString &filePath)
{
    QMimeDatabase mimeDb;
    QMimeType mimeType = mimeDb.mimeTypeForFile(filePath);

    return mimeType.name().startsWith("audio/");
}

bool Common::isMediaFile(const QString &filePath)
{
    return isVideoFile(filePath) || isAudioFile(filePath);
}

QSet<QString> Common::supportedVideoMimeTypes()
{
    return getSupportedVideoMimeTypes();
}

QSet<QString> Common::supportedVideoExtensions()
{
    return getSupportedVideoExtensions();
}

QStringList Common::extractSupportedMediaFiles(const QMimeData *mimeData)
{
    QStringList supportedFiles;

    if (!mimeData || !mimeData->hasUrls()) {
        return supportedFiles;
    }

    QList<QUrl> urlList = mimeData->urls();
    for (const QUrl &url : urlList) {
        QString filePath = url.toLocalFile();
        if (!filePath.isEmpty() && isSupportedVideoFile(filePath)) {
            supportedFiles.append(filePath);
        }
    }

    return supportedFiles;
}

bool Common::containsSupportedMediaFiles(const QMimeData *mimeData)
{
    if (!mimeData || !mimeData->hasUrls()) {
        return false;
    }

    QList<QUrl> urlList = mimeData->urls();
    for (const QUrl &url : urlList) {
        QString filePath = url.toLocalFile();
        if (!filePath.isEmpty() && isSupportedVideoFile(filePath)) {
            return true;
        }
    }

    return false;
}

const QSet<QString> &Common::getSupportedVideoMimeTypes()
{
    static QSet<QString> supportedMimeTypes = {
        "video/mp4",
        "video/x-msvideo",    // AVI
        "video/x-matroska",   // MKV
        "video/quicktime",    // MOV
        "video/x-ms-wmv",     // WMV
        "video/mpeg",         // MPG, MPEG
        "video/x-flv",        // FLV
        "video/webm",         // WEBM
        "video/3gpp",         // 3GP
        "video/mp2t",         // TS, M2TS
        "video/x-m4v",        // M4V
        "video/x-ms-asf",     // ASF
        "video/x-mng",        // MNG
        "video/x-sgi-movie"   // MOV
    };

    return supportedMimeTypes;
}

const QSet<QString> &Common::getSupportedVideoExtensions()
{
    static QSet<QString> supportedExtensions = {
        "mp4", "avi", "mkv", "mov", "wmv", "mpg", "mpeg",
        "flv", "webm", "m4v", "3gp", "ts", "mts", "m2ts",
        "asf", "mng", "qt", "divx", "xvid", "rm", "rmvb",
        "vob", "ogv", "mxf", "mjp", "mjpeg"
    };

    return supportedExtensions;
}

