// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "fileswg.h"
#include "ui_fileswg.h"

FilesWG::FilesWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FilesWG)
    , m_model(new FilesHistoryModel(this))
    , m_contextMenu(nullptr)
    , m_removeAction(nullptr)
    , m_deleteFileAction(nullptr)
    , m_openLocationAction(nullptr)
    , m_copyPathAction(nullptr)
    , m_playAction(nullptr)
{
    ui->setupUi(this);
    
    // Set up the list view with the model
    ui->listView->setModel(m_model);
    
    // Set the model role to use for display
    ui->listView->setModelColumn(0); // Use the first column
    
    // Enable context menu
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // Create context menu
    createContextMenu();
    
    // Install Event Filter
    ui->listView->setAcceptDrops(true);
    ui->listView->installEventFilter(this);

    // Connect signals
    connect(ui->listView, &QListView::doubleClicked, this, &FilesWG::onListViewDoubleClicked);
    connect(ui->listView, &QListView::customContextMenuRequested, this, &FilesWG::onCustomContextMenuRequested);

    // connect to playwg
    connect(ui->listView, &QListView::activated, this, &FilesWG::onListViewActivated);
    connect(ui->listView, &QListView::clicked, this, &FilesWG::onListViewActivated);
    connect(ui->listView, &QListView::doubleClicked, this, &FilesWG::onListViewActivated);
    connect(ui->listView, &QListView::pressed, this, &FilesWG::onListViewActivated);
}

FilesWG::~FilesWG()
{
    delete ui;
}

void FilesWG::addActions(const QList<QAction *> actions)
{
    for (auto action : actions){
        if (action)
            m_contextMenu->addAction(action);
    }
}

void FilesWG::addSubActions(const QString &menu, const QList<QAction *> &actions)
{
    QMenu * tmpMenu = m_contextMenu->addMenu(menu);

    for (auto action : actions){
        if (action)
            tmpMenu->addAction(action);
    }
}

void FilesWG::addMenus(const QList<QMenu *> menus)
{
    for (auto menu : menus){;
        if (menu)
            m_contextMenu->addMenu(menu);
    }
}

void FilesWG::addFileToHistory(const QString &filePath)
{
    m_model->addFile(filePath);
}

QString FilesWG::getCurrentSelectFileName()
{
    int index = ui->listView->currentIndex().row();

    return m_model->currentFile(index);
}

void FilesWG::onListViewDoubleClicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString filePath = index.data(FilesHistoryModel::FilePathRole).toString();
        if (!filePath.isEmpty()) {
            m_model->playFile(index.row());
        }
    }
}

void FilesWG::onListViewActivated(const QModelIndex &index)
{
    if (index.isValid()) {
        QString filePath = index.data(FilesHistoryModel::FilePathRole).toString();
        if (!filePath.isEmpty()) {
            emit currentFileActived(QPair<QString, QString>(index.data(FilesHistoryModel::FileNameRole).toString(), filePath));
        }
    }
}

void FilesWG::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->listView->indexAt(pos);
    if (index.isValid()) {
        m_contextMenu->popup(ui->listView->viewport()->mapToGlobal(pos));
    }
}

void FilesWG::createContextMenu()
{
    m_contextMenu = new QMenu(this);
    
    // Remove record action
    m_removeAction = new QAction("Remove Record", this);
    connect(m_removeAction, &QAction::triggered, this, [this]() {
        m_model->removeRecords(ui->listView->selectionModel()->selectedIndexes());
    });
    m_contextMenu->addAction(m_removeAction);
    
    // Delete file action
    m_deleteFileAction = new QAction("Delete File", this);
    connect(m_deleteFileAction, &QAction::triggered, this, [this]() {        
        for (auto index : ui->listView->selectionModel()->selectedIndexes()) {
            if (index.isValid()) {
                m_model->deleteFile(index.row());
            }
        }
    });
    m_contextMenu->addAction(m_deleteFileAction);
    
    m_contextMenu->addSeparator();
    
    // Open file location action
    m_openLocationAction = new QAction("Open File Location", this);
    connect(m_openLocationAction, &QAction::triggered, this, [this]() {
        m_model->openFileLocations(ui->listView->selectionModel()->selectedIndexes());
    });
    m_contextMenu->addAction(m_openLocationAction);
    
    // Copy file path action
    m_copyPathAction = new QAction("Copy File Path", this);
    connect(m_copyPathAction, &QAction::triggered, this, [this]() {
        m_model->copyFilePaths(ui->listView->selectionModel()->selectedIndexes());
    });
    m_contextMenu->addAction(m_copyPathAction);
    
    m_contextMenu->addSeparator();
    
    // Play file action
    m_playAction = new QAction("Play", this);
    connect(m_playAction, &QAction::triggered, this, [this]() {
        for (auto index : ui->listView->selectionModel()->selectedIndexes()) {
            if (index.isValid()) {
                m_model->playFile(index.row());
            }
        }
    });
    m_contextMenu->addAction(m_playAction);
}

void FilesWG::onFilesDropped(const QStringList &filePaths)
{
    for (const QString &filePath : filePaths) {
        addFileToHistory(filePath);
    }
}

bool FilesWG::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == ui->listView) {
        switch (ev->type()) {
        case QEvent::DragEnter:
            dragEnterEvent(static_cast<QDragEnterEvent*>(ev));
            return true;
        case QEvent::DragMove:
            dragMoveEvent(static_cast<QDragMoveEvent*>(ev));
            return true;
        case QEvent::Drop:
            dropEvent(static_cast<QDropEvent*>(ev));
            return true;
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void FilesWG::dragEnterEvent(QDragEnterEvent *event)
{
    if (Common::containsSupportedMediaFiles(event->mimeData())) {
        event->acceptProposedAction();
    } else {
        event->ignore();
    }

    // if (event->mimeData()->hasUrls()) {
    //     // check file type
    //     QList<QUrl> urls = event->mimeData()->urls();
    //     for (const QUrl &url : urls) {
    //         QString filePath = url.toLocalFile();
    //         if (filePath.endsWith(".mp4") || filePath.endsWith(".avi") ||
    //             filePath.endsWith(".mkv")) {
    //             event->acceptProposedAction();
    //             return;
    //         }
    //     }
    // }
    // event->ignore();
}

void FilesWG::dropEvent(QDropEvent *event)
{
    QStringList filePaths = Common::extractSupportedMediaFiles(event->mimeData());
    if (!filePaths.isEmpty()) {
        onFilesDropped(filePaths);
        event->acceptProposedAction();
    } else {
        event->ignore();
    }
}
