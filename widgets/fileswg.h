#ifndef FILESWG_H
#define FILESWG_H

#include <QWidget>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QMenu>
#include <QAction>

#include <common/singleton.h>
#include <model/fileshistorymodel.h>

namespace Ui {
class FilesWG;
}

class FilesWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_SINGLETON(FilesWG)
    explicit FilesWG(QWidget *parent = nullptr);
    ~FilesWG();
    
public slots:
    // Method to add a file to the history, can be called from other parts of the application
    void addFileToHistory(const QString &filePath);

    QString getCurrentSelectFileName();
private slots:
    void onListViewDoubleClicked(const QModelIndex &index);
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    Ui::FilesWG *ui;
    FilesHistoryModel *m_model;
    QMenu *m_contextMenu;
    QAction *m_removeAction;
    QAction *m_deleteFileAction;
    QAction *m_openLocationAction;
    QAction *m_copyPathAction;
    QAction *m_playAction;
    QMenu *m_mediaInfoSubMenu; // Submenu for Media Info actions
    QList<QAction*> m_mediaInfoActions; // To hold the actions for the submenu

    void createContextMenu();
    void createMediaInfoSubMenu(); // Helper to create the submenu

signals:
    // Signal to request media info display for a specific file and command
    void requestMediaInfoDisplay(const QString &filePath, const QString &ffprobeCommand, const QString &windowTitle, const QString &formatType);
};

#endif // FILESWG_H
