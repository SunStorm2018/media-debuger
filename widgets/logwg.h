#ifndef LOGWG_H
#define LOGWG_H

#include <QWidget>

#include <common/singleton.h>
#include <common/tableheadermanager.h>
#include <common/ztexthighlighter.h>

#include <widgets/searchwg.h>

class LogModel;

namespace Ui {
class LogWG;
}

class LogWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_SINGLETON(LogWG)
    LogWG(QWidget *parent = nullptr);
    ~LogWG();

public slots:
    void outLog(const QString &log);

private slots:
    void showContextMenu(const QPoint &pos);
    void toggleSearchDetail();
    void toggleView();
    void on_searchReady();

private:
    Ui::LogWG *ui;
    LogModel *m_logModel;
    TableHeaderManager *m_headerManager;

    ZTextHighlighter *m_highLighter;
    SearchWG *m_searchWG;
};

#endif // LOGWG_H
