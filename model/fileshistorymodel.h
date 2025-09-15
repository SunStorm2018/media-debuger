#ifndef FILESHISTORYMODEL_H
#define FILESHISTORYMODEL_H

#include <QAbstractListModel>
#include <QSettings>
#include <QFileInfo>
#include <QDesktopServices>
#include <QApplication>
#include <QClipboard>
#include <QProcess>
#include <QMenu>
#include <QContextMenuEvent>

#include <common/common.h>

class FilesHistoryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum FileRoles {
        FilePathRole = Qt::UserRole + 1,
        FileNameRole
    };

    explicit FilesHistoryModel(QObject *parent = nullptr);
    ~FilesHistoryModel();

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add/remove data:
    bool addFile(const QString &filePath);
    bool removeFile(const QString &filePath);
    bool removeFile(int index);
    void clearHistory();

    // Load/Save settings
    void loadSettings();
    void saveSettings();

    // Context menu actions
    void removeRecord(int index);
    void deleteFile(int index);
    void openFileLocation(int index);
    void copyFilePath(int index);
    void playFile(int index);

private:
    QStringList m_filePaths;
    QSettings m_settings;
    constexpr static auto FILES_KEY = "RecentFiles";
};

#endif // FILESHISTORYMODEL_H
