// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

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

    saveSettings();
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

    saveSettings();
    return true;
}

void FilesHistoryModel::clearHistory()
{
    beginResetModel();
    m_filePaths.clear();
    endResetModel();
    saveSettings();
}

void FilesHistoryModel::loadSettings()
{
    m_settings.beginGroup(RECENTFILES_SETTINGS_GROUP);
    m_filePaths = m_settings.value(FILES_KEY).toStringList();
    m_settings.endGroup();

    QStringList validFilePaths;
    for (const QString &filePath : m_filePaths) {
        if (QFileInfo::exists(filePath)) {
            validFilePaths.append(filePath);
        } else {
            qWarning() << "The file not exist, which will be removed: " << filePath;
        }
    }
    
    if (validFilePaths.size() != m_filePaths.size()) {
        m_filePaths = validFilePaths;
        saveSettings();
    }
}

void FilesHistoryModel::saveSettings()
{
    m_settings.beginGroup(RECENTFILES_SETTINGS_GROUP);
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

void FilesHistoryModel::removeRecords(const QModelIndexList& indexs)
{
    if (indexs.isEmpty()) {
        return;
    }
    int firstRow = indexs.first().row();
    int lastRow = indexs.last().row();

    beginRemoveRows(QModelIndex(), firstRow, lastRow);

    for (int i = lastRow; i >= firstRow; --i) {
        if (i < m_filePaths.size()) {
            m_filePaths.removeAt(i);
        }
    }

    endRemoveRows();

    saveSettings();
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

void FilesHistoryModel::openFileLocations(const QModelIndexList &indexs)
{
    if (indexs.isEmpty()) {
        return;
    }

    // Use QSet to store unique parent directories to avoid duplicates
    QSet<QString> uniqueParentDirs;
    QList<QString> validFilePaths;

    // Collect valid file paths and their parent directories
    for (const QModelIndex &index : indexs) {
        if (!index.isValid() || index.row() < 0 || index.row() >= m_filePaths.size()) {
            continue;
        }

        QString filePath = m_filePaths.at(index.row());
        QFileInfo fileInfo(filePath);

        if (!fileInfo.exists()) {
            QMessageBox::warning(nullptr, "File Not Found",
                                 QString("The file does not exist:\n%1").arg(filePath));
            continue;
        }

        QString parentDir = fileInfo.absolutePath();
        if (!uniqueParentDirs.contains(parentDir)) {
            uniqueParentDirs.insert(parentDir);
            validFilePaths.append(filePath);
        }
    }

    if (validFilePaths.isEmpty()) {
        return;
    }

    // Open file locations for each unique parent directory
    bool overallSuccess = false;
    for (const QString &filePath : validFilePaths) {
        QFileInfo fileInfo(filePath);
        bool success = false;

#if defined(Q_OS_WIN)
        success = QProcess::startDetached("explorer.exe",
                                          QStringList() << "/select," << QDir::toNativeSeparators(filePath));
#elif defined(Q_OS_MACOS)
        success = QProcess::startDetached("open",
                                          QStringList() << "-R" << filePath);
#else // Linux and other OS \
    // Try to find a suitable file manager
        if (QProcess::execute("which", QStringList() << "dolphin") == 0) {
            success = QProcess::startDetached("dolphin",
                                              QStringList() << "--select" << filePath);
        } else if (QProcess::execute("which", QStringList() << "nautilus") == 0) {
            success = QProcess::startDetached("nautilus",
                                              QStringList() << "--select" << filePath);
        } else if (QProcess::execute("which", QStringList() << "thunar") == 0) {
            success = QProcess::startDetached("thunar",
                                              QStringList() << "--select" << filePath);
        } else if (QProcess::execute("which", QStringList() << "xdg-open") == 0) {
            // Fallback: open the parent directory instead of selecting the file
            success = QProcess::startDetached("xdg-open",
                                              QStringList() << fileInfo.absolutePath());
        }
#endif

        if (success) {
            overallSuccess = true;
        }
    }

    if (!overallSuccess) {
        QMessageBox::warning(nullptr, "Error", "Could not open file location(s).");
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

void FilesHistoryModel::copyFilePaths(QList<int> indexs)
{
    QString filePaths;
    for (auto index: indexs) {
        if (index < 0 || index >= m_filePaths.size()) {
            continue;
        }
        filePaths.append(m_filePaths.at(index) + "\n");
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(filePaths);
}

void FilesHistoryModel::copyFilePaths(QModelIndexList indexs)
{
    QString filePaths;
    for (auto index: indexs) {
        filePaths.append(m_filePaths.at(index.row()) + "\n");
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(filePaths);
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
