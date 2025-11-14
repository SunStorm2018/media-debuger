// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "logmodel.h"
#include <QRegularExpression>

LogModel::LogModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int LogModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_logEntries.size();
}

int LogModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_logEntries.size())
        return QVariant();

    const LogEntry &entry = m_logEntries.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case Time:
            return entry.timestamp.toString("yyyy-MM-dd hh:mm:ss.zzz");
        case Level:
            return entry.level;
        case Model:
            return entry.module;
        case Info:
            return entry.info;
        case File:
            return entry.file;
        case Line:
            return entry.line;
        case Function:
            return entry.function;
        default:
            return QVariant();
        }
    }
    
    if (role == Qt::TextAlignmentRole) {
        return static_cast<int>(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return getColumnName(section);
    }
    
    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return QString::number(section + 1);
    }
    
    return QVariant();
}

void LogModel::addLogEntry(const QString &rawLog)
{
    if (rawLog.trimmed().isEmpty())
        return;

    LogEntry entry = parseLogEntry(rawLog);
    
    beginInsertRows(QModelIndex(), m_logEntries.size(), m_logEntries.size());
    m_logEntries.append(entry);
    endInsertRows();
}

void LogModel::clearLogs()
{
    if (m_logEntries.isEmpty())
        return;
        
    beginResetModel();
    m_logEntries.clear();
    endResetModel();
}

LogEntry LogModel::parseLogEntry(const QString &rawLog)
{
    LogEntry entry;
    
    // Pattern for full format: [timestamp] [level] [module] message [File] file [Line] line [Fun] function
    static QRegularExpression fullPattern(R"(\[([^\]]+)\]\s*\[([^\]]+)\]\s*\[([^\]]+)\]\s*(.+?)\s*\[File\]\s*([^\s]+)\s*\[Line\]\s*(\d+)\s*\[Fun\]\s*(.+))");
    
    // Pattern for simple format: [timestamp] [level] [file:line] message
    static QRegularExpression simplePattern(R"(\[([^\]]+)\]\s*\[([^\]]+)\]\s*\[([^:]+):(\d+)\]\s*(.+))");
    
    QRegularExpressionMatch fullMatch = fullPattern.match(rawLog);
    if (fullMatch.hasMatch()) {
        // Full format parsing
        entry.timestamp = QDateTime::fromString(fullMatch.captured(1), "yyyy-MM-dd hh:mm:ss.zzz");
        entry.level = fullMatch.captured(2).toUpper();
        entry.module = fullMatch.captured(3);
        entry.info = fullMatch.captured(4).trimmed();
        entry.file = fullMatch.captured(5);
        entry.line = fullMatch.captured(6);
        entry.function = fullMatch.captured(7);
        return entry;
    }
    
    QRegularExpressionMatch simpleMatch = simplePattern.match(rawLog);
    if (simpleMatch.hasMatch()) {
        // Simple format parsing
        entry.timestamp = QDateTime::fromString(simpleMatch.captured(1), "yyyy-MM-dd hh:mm:ss.zzz");
        entry.level = simpleMatch.captured(2).toUpper();
        entry.file = simpleMatch.captured(3);
        entry.line = simpleMatch.captured(4);
        entry.info = simpleMatch.captured(5).trimmed();
        
        // Extract module name from file path
        QString filePath = entry.file;
        int lastSlash = filePath.lastIndexOf('/');
        int lastBackslash = filePath.lastIndexOf('\\');
        int lastSeparator = qMax(lastSlash, lastBackslash);
        
        if (lastSeparator != -1) {
            QString fileName = filePath.mid(lastSeparator + 1);
            int dotIndex = fileName.lastIndexOf('.');
            if (dotIndex != -1) {
                entry.module = fileName.left(dotIndex);
            } else {
                entry.module = fileName;
            }
        } else {
            // No path separator, extract from filename directly
            int dotIndex = filePath.lastIndexOf('.');
            if (dotIndex != -1) {
                entry.module = filePath.left(dotIndex);
            } else {
                entry.module = filePath;
            }
        }
        
        return entry;
    }
    
    // Fallback: if no pattern matches, treat as raw message
    entry.timestamp = QDateTime::currentDateTime();
    entry.level = "INFO";
    entry.module = "Unknown";
    entry.info = rawLog.trimmed();
    entry.file = "";
    entry.line = "";
    entry.function = "";
    
    return entry;
}

QString LogModel::getColumnName(int column) const
{
    switch (column) {
    case Time:
        return "Time";
    case Level:
        return "Level";
    case Model:
        return "Model";
    case Info:
        return "Info";
    case File:
        return "File";
    case Line:
        return "Line";
    case Function:
        return "Function";
    default:
        return QString("Column %1").arg(column);
    }
}
