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
#include <common/singleton.h>

#include "x11embedhelper.h"

class X11EmbedHelper;

namespace Ui {
class PlayerWG;
}

class PlayerWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_SINGLETON(PlayerWG)
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

private:
    void initConnections();
    void startFfplay();
    void stopFfplay();
    void embedFfplayWindow();
    void resizeFfplayWindow();
    void sendKeyToFfplay(const QString &key);
    void sendMouseToFfplay(const QPoint pos, const MouseButton button);

    Ui::PlayerWG *ui;
    QProcess *m_ffplayProcess;
    QTimer *m_positionTimer;
    QString m_mediaFile;
    bool m_isPlaying;
    bool m_isPaused;
    int m_volume;
    unsigned long m_ffplayWindow;
    X11EmbedHelper *m_embedHelper;
    bool m_windowEmbedded;
    int m_embedRetryCount;
    double m_currentRelativePosition; // Save current relative position

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // PLAYERWG_H
