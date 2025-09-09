#ifndef TBHEADEREDITOR_H
#define TBHEADEREDITOR_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class TBHeaderEditor;
}

class TBHeaderEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TBHeaderEditor(QWidget *parent = nullptr);
    ~TBHeaderEditor();

    void loadHeaders(const QStringList& headers);

    QStringList getSelectedHeaders() const;
    QStringList getCurrentOrder() const;
signals:
    void selectionChanged(const QStringList &selectedHeaders);
    void orderChanged(const QStringList &newOrder);

private slots:
    void onItemSelectionChanged();
    void onItemMoved();

private:
    Ui::TBHeaderEditor *ui;
};

#endif // TBHEADEREDITOR_H
