// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef PLAYERWG_H
#define PLAYERWG_H

#include <QWidget>
#include <QProcess>
#include <QTimer>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include <QApplication>
#include <common/zsingleton.h>
#include <QMutex>
#include <QJsonDocument>
#include <QJsonObject>

#include "../common/zffprobe.h"

#include "../common/zx11embedhelper.h"

class ZX11EmbedHelper;

namespace Ui {
class PlayerWG;
}

class PlayerWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_ZSINGLETON(PlayerWG)
    explicit PlayerWG(QWidget *parent = nullptr);
    ~PlayerWG();

public:
    void setMediaFile(const QString &filePath);
    void play();
    void pause();
    void stop();
    void seek(int position);
    void setVolume(int volume);

signals:
    void durationChanged(int duration);
    void stateChanged(bool isPlaying);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onPlayPauseClicked();
    void onStopClicked();
    void onVolumeSpinBoxChanged(int value);
    void onFfplayFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onPositionTimerTimeout();
    void onX11KeyEvent(int keySym, unsigned long windowId);

private:
    void initConnections();
    void startFfplay();
    void stopFfplay();
    void embedFfplayWindow();
    void resizeFfplayWindow();
    void sendKeyToFfplay(const QString &key);
    void sendMouseToFfplay(const QPoint pos, const ZMouseButton button);
    // x_root,y_root are global root window coordinates
    void onMouseEventFromX11(int x_root, int y_root, int windowWidth, int windowHeight, unsigned long windowId);

    Ui::PlayerWG *ui;
    QProcess *m_ffplayProcess;
    QTimer *m_positionTimer;
    ZFfprobe *m_probe;
    QString m_mediaFile;
    bool m_isPlaying;
    bool m_isPaused;
    int m_volume;
    unsigned long m_ffplayWindow;
    ZX11EmbedHelper *m_embedHelper;
    bool m_windowEmbedded;
    int m_embedRetryCount;
    double m_currentRelativePosition; // Save current relative position
    QMutex m_progressMutex;
    double m_durationSeconds; // media duration in seconds (0 unknown)

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // PLAYERWG_H
