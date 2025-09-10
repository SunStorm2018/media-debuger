#include "logwg.h"
#include "ui_logwg.h"
#include <QMenu>
#include <QShortcut>

LogWG::LogWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogWG)
{
    ui->setupUi(this);
    ui->end_time_dte->setVisible(false);
    ui->stackedWidget->setCurrentWidget(ui->log_text_wg);
    ui->search_detail_wg->setVisible(false);
    
    // Setup context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &LogWG::customContextMenuRequested, this, &LogWG::showContextMenu);

    // Setup shortcuts
    new QShortcut(QKeySequence("Ctrl+D"), this, SLOT(toggleSearchDetail()));
    new QShortcut(QKeySequence("Ctrl+T"), this, SLOT(toggleView()));
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
    delete ui;
}

void LogWG::outLog(const QString &log)
{
    ui->log_ple->appendPlainText(log);
}

void LogWG::on_expand_time_btn_clicked()
{
    ui->end_time_dte->setVisible(ui->expand_time_btn->isChecked());
}


void LogWG::on_use_regular_express_cbx_clicked(bool checked)
{

}

