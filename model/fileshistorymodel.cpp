#include "fileshistorymodel.h"
#include <QDir>
#include <QMessageBox>

FilesHistoryModel::FilesHistoryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_settings(ORGANIZATION_NAME, APPLICATION_NAME) // Replace with your actual organization and app name
{
    loadSettings();
}

FilesHistoryModel::~FilesHistoryModel()
{
    saveSettings();
}

QVariant FilesHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant(); // No header for a simple list
}

int FilesHistoryModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, return rowCount so that any child nodes can be included.
    if (parent.isValid())
        return 0;

    return m_filePaths.size();
}

QVariant FilesHistoryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_filePaths.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole || role == FileNameRole) {
        QFileInfo fileInfo(m_filePaths.at(index.row()));
        return fileInfo.fileName();
    } else if (role == FilePathRole) {
        return m_filePaths.at(index.row());
    } else if (role == Qt::ToolTipRole) {
        return m_filePaths.at(index.row()); // Show full path as tooltip
    }

    return QVariant();
}

bool FilesHistoryModel::addFile(const QString &filePath)
{
    if (filePath.isEmpty() || !QFileInfo::exists(filePath)) {
        return false;
    }

    // Avoid duplicates
    if (m_filePaths.contains(filePath)) {
        // Move to top if already exists
        m_filePaths.removeAll(filePath);
    }

    beginInsertRows(QModelIndex(), 0, 0);
    m_filePaths.prepend(filePath);
    endInsertRows();

    // Keep only the last N files, e.g., 50
    const int maxFiles = 50;
    if (m_filePaths.size() > maxFiles) {
        beginRemoveRows(QModelIndex(), maxFiles, m_filePaths.size() - 1);
        m_filePaths.mid(0, maxFiles);
        endRemoveRows();
    }

    saveSettings(); // Save immediately
    return true;
}

bool FilesHistoryModel::removeFile(const QString &filePath)
{
    int index = m_filePaths.indexOf(filePath);
    if (index != -1) {
        return removeFile(index);
    }
    return false;
}

bool FilesHistoryModel::removeFile(int index)
{
    if (index < 0 || index >= m_filePaths.size()) {
        return false;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_filePaths.removeAt(index);
    endRemoveRows();

    saveSettings(); // Save immediately
    return true;
}

void FilesHistoryModel::clearHistory()
{
    beginResetModel();
    m_filePaths.clear();
    endResetModel();
    saveSettings(); // Save immediately
}

void FilesHistoryModel::loadSettings()
{
    m_settings.beginGroup(RECENTFILES_GROUP);
    m_filePaths = m_settings.value(FILES_KEY).toStringList();
    m_settings.endGroup();
}

void FilesHistoryModel::saveSettings()
{
    m_settings.beginGroup(RECENTFILES_GROUP);
    m_settings.setValue(FILES_KEY, m_filePaths);
    m_settings.endGroup();
}

QString FilesHistoryModel::currentFile(int index)
{
    if (index >= 0 && index < m_filePaths.size()) {
        return m_filePaths.at(index);
    } else {
        return "";
    }
}

void FilesHistoryModel::removeRecord(int index)
{
    removeFile(index);
}

void FilesHistoryModel::deleteFile(int index)
{
    if (index < 0 || index >= m_filePaths.size()) {
        return;
    }
    QString filePath = m_filePaths.at(index);

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "Delete File",
                                  QString("Are you sure you want to delete the file:\n%1?").arg(filePath),
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QFile file(filePath);
        if (file.remove()) {
            removeFile(index); // Remove from history after successful deletion
        } else {
            QMessageBox::warning(nullptr, "Delete Failed", "Failed to delete the file.");
        }
    }
}

void FilesHistoryModel::openFileLocation(int index)
{
    if (index < 0 || index >= m_filePaths.size()) {
        return;
    }
    QString filePath = m_filePaths.at(index);
    QFileInfo fileInfo(filePath);

    if (!fileInfo.exists()) {
        QMessageBox::warning(nullptr, "File Not Found", "The file does not exist.");
        return;
    }

    // Select the file in the file explorer
    bool success = false;
#if defined(Q_OS_WIN)
    success = QProcess::startDetached("explorer.exe", QStringList() << "/select," << QDir::toNativeSeparators(filePath));
#elif defined(Q_OS_MACOS)
    success = QProcess::startDetached("open", QStringList() << "-R" << filePath);
#else // Linux and other OS
    // Try to find a suitable file manager
    if (QProcess::execute("which", QStringList() << "dolphin") == 0) {
        success = QProcess::startDetached("dolphin", QStringList() << "--select" << filePath);
    } else if (QProcess::execute("which", QStringList() << "nautilus") == 0) {
        success = QProcess::startDetached("nautilus", QStringList() << "--select" << filePath);
    } else if (QProcess::execute("which", QStringList() << "xdg-open") == 0) {
        // Fallback: open the directory
        success = QProcess::startDetached("xdg-open", QStringList() << fileInfo.absolutePath());
    }
#endif

    if (!success) {
        QMessageBox::warning(nullptr, "Error", "Could not open file location.");
    }
}

void FilesHistoryModel::copyFilePath(int index)
{
    if (index < 0 || index >= m_filePaths.size()) {
        return;
    }
    QString filePath = m_filePaths.at(index);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(filePath);
}

void FilesHistoryModel::playFile(int index)
{
    if (index < 0 || index >= m_filePaths.size()) {
        return;
    }
    QString filePath = m_filePaths.at(index);

    if (!QFileInfo::exists(filePath)) {
        QMessageBox::warning(nullptr, "File Not Found", "The file does not exist.");
        return;
    }

    bool success = QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    if (!success) {
        QMessageBox::warning(nullptr, "Error", "Could not open file with the default player.");
    }
}
