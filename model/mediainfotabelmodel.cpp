#include "mediainfotabelmodel.h"

MediaInfoTabelModel::MediaInfoTabelModel(QObject *parent) : QAbstractTableModel(parent)
{

}

int MediaInfoTabelModel::rowCount(const QModelIndex &parent) const
{
    return row;
}

int MediaInfoTabelModel::columnCount(const QModelIndex &parent) const
{
    return column;
}

QVariant MediaInfoTabelModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return (*m_data)[index.row()][index.column()];
    }

    if(role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    // if(role == Qt::BackgroundRole &&  index.row() % 2 == 0)
    // {
    //     return QBrush(QColor(50, 50, 50));
    // }

    return QVariant();
}

QVariant MediaInfoTabelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        if(m_header->count() - 1 >= section)
            return m_header->at(section);
    }

    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return QString::number(section + 1);
    }

    //qDebug()<<role << "--" << Qt::BackgroundRole;

    //    if(role == Qt::BackgroundRole)
    //    {
    //        return QBrush(QColor(156, 233, 248));
    //    }
    //    if(role == Qt::ForegroundRole)
    //    {
    //         return QBrush(QColor(156, 233, 248));
    //    }

    if(role == Qt::FontRole)
    {
        return QFont(tr("微软雅黑"),10, QFont::DemiBold);
    }
    return QVariant();
}

bool MediaInfoTabelModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        if (!checkIndex(index))
            return false;

        //save value from editor to member m_gridData
        (*m_data)[index.row()][index.column()] = value.toString();
        return true;
    }
    return false;
}

Qt::ItemFlags MediaInfoTabelModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

void MediaInfoTabelModel::setRow(int newRow)
{
    row = newRow;
}

void MediaInfoTabelModel::setColumn(int newColumn)
{
    column = newColumn;
}

void MediaInfoTabelModel::setTableHeader(QList<QString> *header)
{
    m_header = header;
}

void MediaInfoTabelModel::setTableData(QList<QStringList> *data)
{
    m_data = data;
}

void MediaInfoTabelModel::SlotUpdateTable()
{
    emit dataChanged(createIndex(0, 0), createIndex(row, column), {Qt::DisplayRole});
}
