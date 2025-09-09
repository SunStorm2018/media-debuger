#ifndef INFOWIDGETS_H
#define INFOWIDGETS_H

#include <QWidget>
#include <QAction>
#include <QMenu>

#include <common/common.h>

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

    void init_header_detail_tb(const QStringList &headers);

    void update_data_detail_tb(const QList<QStringList> &data_tb);

    void clear_detail_tb();
private slots:
    void on_search_btn_clicked();

    void on_expand_raw_btn_clicked(bool checked);

    void on_search_le_editingFinished();

    void onHeaderContextMenuRequested(const QPoint &pos);

    void toggleColumnVisibility();

    void showAllColumns();

    void hideAllColumns();

private:
    void format_data(const QString& data, QList<QStringList> &data_tb, QStringList &headers, QString format_key);

private:
    Ui::InfoWidgets *ui;

    QStringList m_headers;

    QList<QStringList> m_data_tb;

    QHash<QAction*, int> m_actionToColumnMap;
};

#endif // INFOWIDGETS_H
