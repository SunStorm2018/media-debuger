// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef INFOTABLEWG_H
#define INFOTABLEWG_H

#include <QWidget>
#include <QAction>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QShortcut>
#include <QClipboard>
#include <QThread>
#include <QTimer>
#include <QHeaderView>
#include <QtConcurrent>

#include <common/common.h>
#include <common/ztableheadermanager.h>
#include <common/zwindowhelper.h>
#include <common/qtcompat.h>

#include <model/mediainfotabelmodel.h>
#include <model/multicolumnsearchproxymodel.h>

#include <widgets/searchwg.h>
#include <widgets/helpquerywg.h>

// Forward declarations
class ProgressDialog;

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

    void setSearchTitleVisiable(const bool& visiable);

    void addContextMenu(QMenu *menu);

    void addContextAction(QAction *action);

    void addContextSeparator();

    QList <QStringList> getSelectLines();

    QList <int> getSelectRows();

    const QList<QStringList> *getTableData();

signals:
    void dataChanged(QStringList line);

public slots:
    void init_detail_tb(const QString& data, const QString &format_key);

    void init_header_detail_tb(const QStringList &headers, QString format_join = "");

    void update_data_detail_tb(const QList<QStringList> &data_tb, QString format_join = "");

    void update_data_detail_tb(const QMap<QString, QList<QStringList>>&data_tb, QString format_join = "");

    void remove_data_from_row_indexs(const QList<int>& indexs);

    void append_data_detail_tb(const QList<QStringList> &data_tb, QString format_join = "");

    void remove_selected_row();

    void clear_detail_tb();

    void copySelectedText();

    void copySelectedTextWithHeader();

    void copySelectedRows();

    void copySelectedRowsWithHeader();

    void copySelectedColumns();

    void copySelectedColumnsWithHeader();

    void copyAllData();

    void copyAllDataWithHeader();

    void fitTableColumnToContent();


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
    QString getSelectedText(bool includeHeader = false);
    void setupContextMenu(); // Setup context menu for table
    void setupCopyMenu(); // Setup copy menu
    void setupTableModel(); // Setup table model and view

private:
    void format_data(const QString& data, QList<QStringList> &data_tb, QStringList &headers, QString format_key);

private:
    Ui::InfoWidgets *ui;

    QStringList m_headers;
    QList<QStringList> m_data_tb;

    MediaInfoTabelModel *m_model;
    MultiColumnSearchProxyModel *multiColumnSearchModel;
    ZTableHeaderManager *m_headerManager;
    
    // Detail search components
    QMenu *m_searchButtonMenu;
    QAction *m_detailSearchAction;

    // Copy
    QMenu *m_copyMenu;
    QAction *m_copySelectedTextAction;
    QAction *m_copySelectedTextWithHeaderAction;
    QAction *m_copySelectedRowsAction;
    QAction *m_copySelectedRowsWithHeaderAction;
    QAction *m_copySelectedColumnsAction;
    QAction *m_copySelectedColumnsWithHeaderAction;
    QAction *m_copyAllDataAction;
    QAction *m_copyAllDataWithHeaderAction;

    // Detail search
    SearchWG *m_detailSearchDialog;

    QMenu *m_tableContextMenu;
    int m_currentRow;
    int m_currentColumn;
    QString m_helpKey;
    QAction *m_detailAction;
    QAction *m_restoreOrderAction;
    QAction *m_fitTableColumnAction;
    
    // Column width management
    QVector<double> m_columnWidthRatios;
    bool m_isUserAdjusted;
    QTimer *m_resizeTimer;
    int m_lastTableWidth;
    
    // Copy operations
    ProgressDialog *m_copyProgressDialog;
    
    void setupColumnWidthManagement();
    void saveColumnWidthRatios();
    void restoreColumnWidthRatios();
    void resizeColumnsProportionally();
    void setupInitialColumnWidths();
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif // INFOTABLEWG_H
