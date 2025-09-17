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
    void positionChanged(int position);
    void durationChanged(int duration);
    void stateChanged(bool isPlaying);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onPlayPauseClicked();
    void onStopClicked();
    void onPositionSliderChanged(int value);
    void onVolumeSliderChanged(int value);
    void onFfplayFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void updatePosition();

private:
    void initConnections();
    void startFfplay();
    void stopFfplay();
    void embedFfplayWindow();
    void resizeFfplayWindow();
    void sendKeyToFfplay(const QString &key);

    Ui::PlayerWG *ui;
    QProcess *m_ffplayProcess;
    QTimer *m_positionTimer;
    QString m_mediaFile;
    bool m_isPlaying;
    bool m_isPaused;
    int m_duration;
    int m_position;
    int m_volume;
    unsigned long m_ffplayWindow;
    X11EmbedHelper *m_embedHelper;
    bool m_windowEmbedded;
    int m_embedRetryCount;
};

#endif // PLAYERWG_H
