#ifndef JSONFORMATWG_H
#define JSONFORMATWG_H

#include <QWidget>
#include <QShortcut>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QClipboard>
#include <QApplication>

#include <widgets/baseformatwg.h>
#include <widgets/searchwg.h>

#include <third_part/QJsonModel/include/QJsonModel.hpp>

namespace Ui {
class JsonFormatWG;
}

class JsonFormatWG : public BaseFormatWG
{
    Q_OBJECT

public:
    explicit JsonFormatWG(QWidget *parent = nullptr);
    ~JsonFormatWG();

private:
    Ui::JsonFormatWG *ui;

    QJsonModel * m_model;
    QSortFilterProxyModel * m_proxyModel;
    SearchWG * m_searchWG;
    QMenu *m_contextMenu;

protected:
    bool loadJson(const QByteArray &json) override;

private slots:
    void on_searchReady();
    void on_searchTextChanged(const QString &text);
    void on_searchClear();
    void showContextMenu(const QPoint &pos);
    void copyValue();
    void copyKeyValue();
    void copyKey();
    void copyAllData();
    void expandAll();
    void collapseAll();

private:
    void countVisibleAndTotalItems(QAbstractItemModel *model, const QModelIndex &parent, int &visibleCount, int &totalCount);
    QString getKeyForIndex(const QModelIndex &index);
    QString getValueForIndex(const QModelIndex &index);
    QJsonTreeItem* getItemForIndex(const QModelIndex &index);
};

#endif // JSONFORMATWG_H
