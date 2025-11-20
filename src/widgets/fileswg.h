// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef FILESWG_H
#define FILESWG_H

#include <QWidget>
#include <QItemSelectionModel>
#include <QModelIndex>
#include <QMenu>
#include <QAction>
#include <QPair>
#include <QListView>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QMimeData>

#include <common/zsingleton.h>
#include <model/fileshistorymodel.h>

namespace Ui {
class FilesWG;
}

class FilesWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_ZSINGLETON(FilesWG)
    explicit FilesWG(QWidget *parent = nullptr);
    ~FilesWG();
    
public:
    void addActions(const QList<QAction*> actions);
    void addSubActions(const QString& menu, const QList<QAction*>& actions);
    void addMenus(const QList<QMenu*> menus);

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
    void onFilesDropped(const QStringList &filePaths);

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // FILESWG_H
