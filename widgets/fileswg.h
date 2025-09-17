#ifndef FILESWG_H
#define FILESWG_H

#include <QWidget>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QMenu>
#include <QAction>
#include <QPair>

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
    
public:
    void addActions(const QList<QAction*> actions);
    void addSubActions(const QString& menu, const QList<QAction*>& actions);

signals:
    void currentFileActived(QPair<QString, QString> filePair);

public slots:
    // Method to add a file to the history, can be called from other parts of the application
    void addFileToHistory(const QString &filePath);

    QString getCurrentSelectFileName();

    void onListViewDoubleClicked(const QModelIndex &index);
    void onListViewActivated(const QModelIndex &index);
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

    void createContextMenu();
};

#endif // FILESWG_H
