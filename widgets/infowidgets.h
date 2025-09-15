#ifndef INFOWIDGETS_H
#define INFOWIDGETS_H

#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QShortcut>

#include <common/common.h>
#include <model/mediainfotabelmodel.h>
#include <model/multicolumnsearchproxymodel.h>
#include <common/tableheadermanager.h>
#include <widgets/searchwg.h>

namespace Ui {
class InfoWidgets;
}

class InfoWidgets : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidgets(QWidget *parent = nullptr);
    ~InfoWidgets();

public slots:
    void init_detail_tb(const QString& data, const QString &format_key);

    void init_header_detail_tb(const QStringList &headers, QString format_join = "");

    void update_data_detail_tb(const QList<QStringList> &data_tb, QString format_join = "");

    void clear_detail_tb();

private slots:
    void on_search_btn_clicked();
    void on_expand_raw_btn_clicked(bool checked);
    void on_search_le_editingFinished();
    
    // Detail search functionality
    void showDetailSearch();
    void onDetailSearchCompleted();

    void on_search_le_textChanged(const QString &arg1);

private:
    void setupSearchButton();
    void createDetailSearchDialog();
    void updateCurrentModel(); // Helper method to update the current active model

private:
    void format_data(const QString& data, QList<QStringList> &data_tb, QStringList &headers, QString format_key);

private:
    Ui::InfoWidgets *ui;

    QStringList m_headers;
    QList<QStringList> m_data_tb;

    MediaInfoTabelModel *model;
    MultiColumnSearchProxyModel *multiColumnSearchModel;
    TableHeaderManager *m_headerManager;
    
    // Detail search components
    QMenu *m_searchButtonMenu;
    QAction *m_detailSearchAction;
    SearchWG *m_detailSearchDialog;
};


#endif // INFOWIDGETS_H
