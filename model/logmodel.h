#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QDebug>

struct LogEntry {
    QDateTime timestamp;
    QString level;
    QString module;
    QString info;
    QString file;
    QString line;
    QString function;
    
    LogEntry() = default;
    LogEntry(const QDateTime& ts, const QString& lv, const QString& mod, 
             const QString& inf, const QString& f = "", const QString& ln = "", const QString& func = "")
        : timestamp(ts), level(lv), module(mod), info(inf), file(f), line(ln), function(func) {}
};

class LogModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        Time = 0,
        Level,
        Model,
        Info,
        File,
        Line,
        Function,
        ColumnCount
    };

    explicit LogModel(QObject *parent = nullptr);

    // QAbstractTableModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Custom methods
    void addLogEntry(const QString &rawLog);
    void clearLogs();

private:
    LogEntry parseLogEntry(const QString &rawLog);
    QString getColumnName(int column) const;

    QList<LogEntry> m_logEntries;
};

#endif // LOGMODEL_H
