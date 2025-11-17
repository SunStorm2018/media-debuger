// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef ZFFMPEG_H
#define ZFFMPEG_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QProcess>
#include <QTemporaryDir>

class ZFFmpeg : public QObject
{
    Q_OBJECT

public:
    explicit ZFFmpeg(QObject *parent = nullptr);
    ~ZFFmpeg();

    // Extract a specific frame from video
    bool extractFrame(const QString &inputFile, int frameNumber, const QString &outputFile);
    
    // Extract frame at specific time
    bool extractFrameAtTime(const QString &inputFile, const QString &time, const QString &outputFile);
    
    // Get video information
    QString getVideoInfo(const QString &inputFile);
    
    // Check if ffmpeg is available
    static bool isAvailable();

private:
    QProcess *m_process;
    
    // Execute ffmpeg command and wait for completion
    bool executeCommand(const QStringList &arguments);
    
    // Generate output filename for frame extraction
    QString generateOutputFilename(const QString &inputFile, int frameNumber);
};

#endif // ZFFMPEG_H