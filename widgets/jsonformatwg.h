#ifndef JSONFORMATWG_H
#define JSONFORMATWG_H

#include <QWidget>
#include <QShortcut>
#include <QSortFilterProxyModel>

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

protected:
    bool loadJson(const QByteArray &json) override;

private slots:
    void on_searchReady();
    void on_searchTextChanged(const QString &text);
    void on_searchClear();

private:
    void countVisibleAndTotalItems(QAbstractItemModel *model, const QModelIndex &parent, int &visibleCount, int &totalCount);
};

#endif // JSONFORMATWG_H
