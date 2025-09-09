#include "tbheadereditor.h"
#include "ui_tbheadereditor.h"

TBHeaderEditor::TBHeaderEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TBHeaderEditor)
{
    ui->setupUi(this);

    ui->header_lw->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->header_lw->setDragDropMode(QAbstractItemView::InternalMove);
    ui->header_lw->setDefaultDropAction(Qt::MoveAction);

    connect(ui->header_lw, &QListWidget::itemSelectionChanged,
            this, &TBHeaderEditor::onItemSelectionChanged);

    connect(ui->header_lw->model(), &QAbstractItemModel::rowsMoved,
            this, &TBHeaderEditor::onItemMoved);
}

TBHeaderEditor::~TBHeaderEditor()
{
    delete ui;
}

void TBHeaderEditor::loadHeaders(const QStringList &headers)
{
    // 阻塞信号，避免多次触发
    ui->header_lw->blockSignals(true);

    ui->header_lw->clear();
    ui->header_lw->addItems(headers);

    // 恢复信号
    ui->header_lw->blockSignals(false);

    // 可选：默认选择所有项
    ui->header_lw->selectAll();
    emit selectionChanged(getSelectedHeaders());
}

QStringList TBHeaderEditor::getSelectedHeaders() const
{
    QStringList selected;
    for (QListWidgetItem *item : ui->header_lw->selectedItems()) {
        selected.append(item->text());
    }
    return selected;
}

QStringList TBHeaderEditor::getCurrentOrder() const
{
    QStringList order;
    for (int i = 0; i < ui->header_lw->count(); ++i) {
        order.append(ui->header_lw->item(i)->text());
    }
    return order;
}

void TBHeaderEditor::onItemSelectionChanged()
{
    emit selectionChanged(getSelectedHeaders());
}

void TBHeaderEditor::onItemMoved()
{
    emit orderChanged(getCurrentOrder());
}
