#ifndef MEDIAINFOTABELMODEL_H
#define MEDIAINFOTABELMODEL_H

#include <QAbstractTableModel>
#include <QDebug>
#include <QColor>
#include <QBrush>
#include <QFont>

class MediaInfoTabelModel: public QAbstractTableModel
{
    Q_OBJECT
public:
    MediaInfoTabelModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setRow(int newRow);

    void setColumn(int newColumn);

    void setTableHeader(QList<QString> *header);

    void setTableData(QList<QStringList> *data);

signals:
    void editCompleted(const QString &);

public slots:
    void SlotUpdateTable();
private:
    int row = 0;
    int column = 0;

    QList<QString> *m_header;
    QList<QStringList> *m_data;
};

#endif // MediaInfoTabelModel_H
