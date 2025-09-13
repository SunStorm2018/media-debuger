#include "logwg.h"
#include "ui_logwg.h"
#include <QMenu>
#include <QShortcut>
#include <model/logmodel.h>

LogWG::LogWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogWG)
    , m_logModel(new LogModel(this))
{
    ui->setupUi(this);
    ui->end_time_dte->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->log_text_wg);
    ui->search_detail_wg->setVisible(false);
    
    // Setup log table view
    ui->log_tbv->setModel(m_logModel);
    ui->log_tbv->setAlternatingRowColors(true);
    ui->log_tbv->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    // Initialize header manager
    ui->log_tbv->horizontalHeader()->setSectionsMovable(true);
    ui->log_tbv->verticalHeader()->setDefaultAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->log_tbv->verticalHeader()->setDefaultSectionSize(25);
    ui->log_tbv->verticalHeader()->setVisible(true);

    m_headerManager = new TableHeaderManager(ui->log_tbv->horizontalHeader(), ui->log_tbv->verticalHeader(), this);
    m_headerManager->setObjectName(this->objectName());
    m_headerManager->enableHeaderContextMenu(true);
    m_headerManager->setTotalCountVisible(false);
    
    // Setup context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &LogWG::customContextMenuRequested, this, &LogWG::showContextMenu);

    // Setup shortcuts
    new QShortcut(QKeySequence("Ctrl+D"), this, SLOT(toggleSearchDetail()));
    new QShortcut(QKeySequence("Ctrl+T"), this, SLOT(toggleView()));
    
    // Restore header state
    m_headerManager->restoreState();
}

void LogWG::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(this);
    
    // Add search detail action
    QAction *searchAction = contextMenu.addAction("Toggle Search Detail (Ctrl+D)");
    connect(searchAction, &QAction::triggered, this, &LogWG::toggleSearchDetail);
    
    // Add view toggle action
    QAction *viewAction = contextMenu.addAction("Toggle View (Ctrl+T)");
    connect(viewAction, &QAction::triggered, this, &LogWG::toggleView);
    
    contextMenu.addSeparator();
    
    // Add clear logs action
    QAction *clearAction = contextMenu.addAction("Clear Logs");
    connect(clearAction, &QAction::triggered, this, [this](){
        ui->log_ple->clear();
        m_logModel->clearLogs();
    });
    
    contextMenu.exec(mapToGlobal(pos));
}

void LogWG::toggleSearchDetail()
{
    // Toggle search detail widget visibility search_detail_wg
    ui->search_detail_wg->setVisible(!ui->search_detail_wg->isVisible());
}

void LogWG::toggleView()
{
    // Toggle between text and table views
    if (ui->stackedWidget->currentWidget() == ui->log_text_wg) {
        ui->stackedWidget->setCurrentWidget(ui->log_table_wg);
    } else {
        ui->stackedWidget->setCurrentWidget(ui->log_text_wg);
    }
}

LogWG::~LogWG()
{
    delete m_headerManager;
    delete ui;
}

void LogWG::outLog(const QString &log)
{
    // Add to text view
    ui->log_ple->appendPlainText(log);
    
    // Add to table model
    m_logModel->addLogEntry(log);
    
    // Update total count
    m_headerManager->updateTotalCount(m_logModel->rowCount());
    
    // Auto-scroll to bottom if table view is visible
    if (ui->stackedWidget->currentWidget() == ui->log_table_wg) {
        ui->log_tbv->scrollToBottom();
    }

    ui->log_tbv->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // Time
    ui->log_tbv->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Level
    ui->log_tbv->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Interactive); // Function (可手动调整)
    ui->log_tbv->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch); // Info (自动伸展)
}

void LogWG::on_expand_time_btn_clicked()
{
    ui->end_time_dte->setVisible(ui->expand_time_btn->isChecked());
}


void LogWG::on_use_regular_express_cbx_clicked(bool checked)
{

}

