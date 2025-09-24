#ifndef INFOWIDGETS_H
#define INFOWIDGETS_H

#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QShortcut>
#include <QClipboard>
#include <QThread>
#include <QTimer>
#include <QHeaderView>

#include <common/common.h>
#include <common/tableheadermanager.h>
#include <common/zwindowhelper.h>

#include <model/mediainfotabelmodel.h>
#include <model/multicolumnsearchproxymodel.h>

#include <widgets/searchwg.h>
#include <widgets/helpquerywg.h>

namespace Ui {
class InfoWidgets;
}

class InfoWidgets : public QWidget
{
    Q_OBJECT

public:
    explicit InfoWidgets(QWidget *parent = nullptr);
    ~InfoWidgets();

    void setHelpInfoKey(const QString& key);

signals:
    void dataChanged(QStringList line);

public slots:
    void init_detail_tb(const QString& data, const QString &format_key);

    void init_header_detail_tb(const QStringList &headers, QString format_join = "");

    void update_data_detail_tb(const QList<QStringList> &data_tb, QString format_join = "");

    void clear_detail_tb();

    void exportSelectedData();
    void copySelectedText();
    void showDetailInfo();

private slots:
    void on_search_btn_clicked();
    void on_expand_raw_btn_clicked(bool checked);
    void on_search_le_editingFinished();
    
    // Detail search functionality
    void showDetailSearch();
    void onDetailSearchCompleted();

    void on_search_le_textChanged(const QString &arg1);
    
    // Column width management slots
    void onHeaderSectionResized(int logicalIndex, int oldSize, int newSize);
    void onResizeTimerTimeout();

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

    MediaInfoTabelModel *m_model;
    MultiColumnSearchProxyModel *multiColumnSearchModel;
    TableHeaderManager *m_headerManager;
    
    // Detail search components
    QMenu *m_searchButtonMenu;
    QAction *m_detailSearchAction;
    SearchWG *m_detailSearchDialog;

    QMenu *m_tableContextMenu;
    int m_currentRow;
    int m_currentColumn;
    QString m_helpKey;
    QAction *m_detailAction;
    QAction *m_restoreOrderAction;
    
    // Column width management
    QVector<double> m_columnWidthRatios;
    bool m_isUserAdjusted;
    QTimer *m_resizeTimer;
    int m_lastTableWidth;
    
    void setupColumnWidthManagement();
    void saveColumnWidthRatios();
    void restoreColumnWidthRatios();
    void resizeColumnsProportionally();
    void setupInitialColumnWidths();
    bool eventFilter(QObject *obj, QEvent *event) override;
};


#endif // INFOWIDGETS_H
