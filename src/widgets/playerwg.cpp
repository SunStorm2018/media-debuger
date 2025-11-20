// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "playerwg.h"
#include "ui_playerwg.h"
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QTime>
#include <QJsonObject>
#include <QThread>
#include <cmath>
#ifdef Q_OS_LINUX
#include <X11/keysym.h>
#endif

PlayerWG::PlayerWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerWG)
    , m_ffplayProcess(nullptr)
    , m_positionTimer(new QTimer(this))
    , m_isPlaying(false)
    , m_isPaused(false)
    , m_volume(50)
    , m_ffplayWindow(0)
    , m_embedHelper(new ZX11EmbedHelper(this))
    , m_windowEmbedded(false)
    , m_embedRetryCount(0)
    , m_currentRelativePosition(0.0)
{
    ui->setupUi(this);
    initConnections();

    if (!m_embedHelper->initialize()) {
        qWarning() << "PlayerWG: Failed to initialize X11 embed helper during construction";
    }

    ui->volumeSpinBox->setValue(m_volume);
    
    m_positionTimer->setInterval(1000);
    m_probe = new ZFfprobe(this);
    m_durationSeconds = 0.0;
}

PlayerWG::~PlayerWG()
{
    stopFfplay();
    delete ui;
}

void PlayerWG::initConnections()
{
    connect(ui->playPauseBtn, &QPushButton::clicked, this, &PlayerWG::onPlayPauseClicked);
    connect(ui->stopBtn, &QPushButton::clicked, this, &PlayerWG::onStopClicked);
    connect(ui->volumeSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &PlayerWG::onVolumeSpinBoxChanged);
    
    // Connect X11 mouse events (global/root coordinates)
    if (m_embedHelper) {
        connect(m_embedHelper, &ZX11EmbedHelper::mouseEventReceivedGlobal, this, &PlayerWG::onMouseEventFromX11);
        connect(m_embedHelper, &ZX11EmbedHelper::keyEventReceivedGlobal, this, &PlayerWG::onX11KeyEvent, Qt::AutoConnection);
    }
    // Position timer updates progress every second
    connect(m_positionTimer, &QTimer::timeout, this, &PlayerWG::onPositionTimerTimeout);
}

void PlayerWG::setMediaFile(const QString &filePath)
{
    m_mediaFile = filePath;
    stop();

    // Probe media duration (in seconds) and set progressbar maximum accordingly
    m_durationSeconds = 0.0;
    if (!m_mediaFile.isEmpty() && m_probe) {
        QString json = m_probe->getMediaInfoJsonFormat(SHOW_FORMAT, m_mediaFile);
        if (!json.isEmpty()) {
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(), &err);
            if (err.error == QJsonParseError::NoError && doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj.contains("format") && obj["format"].isObject()) {
                    QJsonObject fmt = obj["format"].toObject();
                    if (fmt.contains("duration")) {
                        bool ok = false;
                        double d = QString(fmt["duration"].toString()).toDouble(&ok);
                        if (ok && d > 0.0) {
                            m_durationSeconds = d;
                            if (ui && ui->playProgressbar) {
                                ui->playProgressbar->setMinimum(0);
                                ui->playProgressbar->setMaximum(static_cast<int>(std::ceil(m_durationSeconds)));
                                ui->playProgressbar->setValue(0);
                            }
                            emit durationChanged(static_cast<int>(std::ceil(m_durationSeconds)));
                        }
                    }
                }
            }
        }
    }
    // qDebug() << "Media file set to: " + filePath;
}

void PlayerWG::play()
{
    if (m_mediaFile.isEmpty()) {
        qWarning() << "No media file set";
        return;
    }

    if (!m_embedHelper->initialize()) {
        qCritical() << "X11 embed helper not available";
    }

    if (!m_ffplayProcess) {
        // Reset progress when starting playback
        m_currentRelativePosition = 0.0;
        if (ui && ui->playProgressbar) {
            ui->playProgressbar->setValue(0);
        }
        startFfplay();
    } else if (m_isPaused) {
        sendKeyToFfplay("p");
        m_isPaused = false;
        m_isPlaying = true;
        ui->playPauseBtn->setText("Pause");
        m_positionTimer->start();
        emit stateChanged(true);
    }
}

void PlayerWG::pause()
{
    if (m_ffplayProcess && m_isPlaying) {
        sendKeyToFfplay("p");
        m_isPaused = true;
        m_isPlaying = false;
        ui->playPauseBtn->setText("Play");
        m_positionTimer->stop();
        emit stateChanged(false);
    }
}

void PlayerWG::stop()
{
    stopFfplay();
    ui->playPauseBtn->setText("Play");
    m_positionTimer->stop();
    // Reset progress bar and saved relative position when stopping
    m_currentRelativePosition = 0.0;
    if (ui && ui->playProgressbar) {
        ui->playProgressbar->setValue(0);
    }

    emit stateChanged(false);
}

void PlayerWG::seek(int position)
{
    if (m_ffplayProcess) {
        sendKeyToFfplay(QString("s"));
    }
}

void PlayerWG::setVolume(int volume)
{
    if (m_ffplayProcess) {
        if (volume > m_volume) {
            for (int i = 0; i < (volume - m_volume); i++) {
                sendKeyToFfplay("0");
            }
        } else {
            for (int i = 0; i < (m_volume - volume); i++) {
                sendKeyToFfplay("9");
            }
        }
    }

    m_volume = volume;
}

void PlayerWG::onPlayPauseClicked()
{
    if (m_isPlaying) {
        pause();
    } else {
        play();
    }
}

void PlayerWG::onStopClicked()
{
    stop();
}


void PlayerWG::onVolumeSpinBoxChanged(int value)
{
    setVolume(value);
}

void PlayerWG::onFfplayFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode)
    Q_UNUSED(exitStatus)
    
    qDebug() << "FFplay process finished";
    m_ffplayProcess = nullptr;
    m_ffplayWindow = 0;
    m_windowEmbedded = false;
    m_isPlaying = false;
    m_isPaused = false;
    
    ui->playPauseBtn->setText("Play");
    m_positionTimer->stop();
    emit stateChanged(false);
}

void PlayerWG::startFfplay()
{
    if (m_ffplayProcess) {
        stopFfplay();
    }
    
    m_ffplayProcess = new QProcess(this);
    connect(m_ffplayProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &PlayerWG::onFfplayFinished);
    
    QStringList arguments;
    arguments << "-autoexit"
              << "-volume" << QString::number(m_volume);

    QString windowTitle = QString("ffplay_%1").arg(QFileInfo(m_mediaFile).baseName());
    arguments << "-window_title" << windowTitle;

    if (m_embedHelper && m_embedHelper->initialize()) {
        arguments << "-noborder";
        QSize videoSize = ui->videoWidget->size();
        if (videoSize.width() > 100 && videoSize.height() > 100) {
            arguments << "-x" << QString::number(videoSize.width());
            arguments << "-y" << QString::number(videoSize.height());
        }
    }
    
    arguments << m_mediaFile;
    
    qDebug() << "PlayerWG: Starting FFplay with arguments:" << arguments.join(" ");
    
    m_ffplayProcess->start("ffplay", arguments);
    
    if (!m_ffplayProcess->waitForStarted(5000)) {
        qCritical() << "PlayerWG: Failed to start FFplay process";
        delete m_ffplayProcess;
        m_ffplayProcess = nullptr;
        return;
    }
    
    m_isPlaying = true;
    m_isPaused = false;
    ui->playPauseBtn->setText("Pause");
    m_positionTimer->start();
    emit stateChanged(true);

    QTimer::singleShot(200, this, &PlayerWG::embedFfplayWindow);
}

void PlayerWG::stopFfplay()
{
    // Stop event monitoring before stopping the process
    if (m_embedHelper) {
        m_embedHelper->stopEventMonitoring();
    }
    
    if (m_ffplayProcess) {
        m_ffplayProcess->kill();
        m_ffplayProcess->waitForFinished(3000);
        m_ffplayProcess->deleteLater();
        m_ffplayProcess = nullptr;
    }
    
    m_ffplayWindow = 0;
    m_windowEmbedded = false;
    m_isPlaying = false;
    m_isPaused = false;
    m_embedRetryCount = 0;
}

void PlayerWG::embedFfplayWindow()
{
    if (!m_ffplayProcess) {
        return;
    }

    if (m_embedRetryCount >= 20) {
        qCritical() << "PlayerWG: Failed to find ffplay window after 20 retries. FFplay is running but not embedded.";
        m_embedRetryCount = 0;
        return;
    }

    QString baseTitle = QString("ffplay_%1").arg(QFileInfo(m_mediaFile).baseName());
    QString fileName = QFileInfo(m_mediaFile).fileName();
    
    if (m_embedRetryCount == 0) {
        qDebug() << "PlayerWG: Starting window search for ffplay embedding";
        qDebug() << "PlayerWG: Looking for window titles:" << baseTitle << "or containing" << fileName;
    }

    m_ffplayWindow = m_embedHelper->findWindow(baseTitle);

    if (m_ffplayWindow == 0) {
        m_ffplayWindow = m_embedHelper->findWindow(fileName);
    }

    if (m_ffplayWindow == 0) {
        m_ffplayWindow = m_embedHelper->findWindow("ffplay");
    }
    
    if (m_ffplayWindow == 0) {
        m_embedRetryCount++;
        QTimer::singleShot(250, this, &PlayerWG::embedFfplayWindow);
        return;
    }

    m_embedRetryCount = 0;
    
    unsigned long parentWindow = ui->videoWidget->winId();
    qInfo() << "PlayerWG: Found ffplay window" << m_ffplayWindow << ", attempting to embed into parent" << parentWindow;
    
    if (m_embedHelper->embedWindow(m_ffplayWindow, parentWindow)) {
        m_windowEmbedded = true;
        resizeFfplayWindow();
        qInfo() << "PlayerWG: FFplay window embedded successfully";

        m_embedHelper->showWindow(m_ffplayWindow);
        
        // Start monitoring mouse events on the embedded window
        m_embedHelper->startEventMonitoring(m_ffplayWindow);
    } else {
        qCritical() << "PlayerWG: Failed to embed FFplay window";
        m_windowEmbedded = false;
    }
}

void PlayerWG::resizeFfplayWindow()
{
    if (m_ffplayWindow == 0 || !m_windowEmbedded) {
        return;
    }
    
    QSize size = ui->videoWidget->size();
    m_embedHelper->resizeWindow(m_ffplayWindow, size.width(), size.height());
}

void PlayerWG::sendKeyToFfplay(const QString &key)
{
    if (m_ffplayWindow == 0) {
        return;
    }
    
    if (!m_embedHelper->sendKey(m_ffplayWindow, key)) {
        qWarning() << "Failed to send key to FFplay: " + key;
    }
}

void PlayerWG::sendMouseToFfplay(const QPoint pos, const ZMouseButton button)
{
    if (m_ffplayWindow == 0) {
        return;
    }
    qDebug() << "send mouse click to FFplay: " << pos << button;
    if (!m_embedHelper->sendMouseClick(m_ffplayWindow, pos.x(), pos.y(), button)) {
        qWarning() << "Failed to send mouse click to FFplay: " << pos << button;
    }
}

void PlayerWG::mousePressEvent(QMouseEvent *event)
{
    // Get mouse position and window size
    QPoint mousePos = event->pos();
    QSize windowSize = this->size();

    // Print detailed information
    // qDebug() << "=== Mouse Click Event ===";
    // qDebug() << "Mouse Position - X:" << mousePos.x() << "Y:" << mousePos.y();
    // qDebug() << "Window Size - Width:" << windowSize.width() << "Height:" << windowSize.height();
    
    // Calculate relative position percentage
    double relativePosition = (double)mousePos.x() / windowSize.width();
    // qDebug() << "Relative Position:" << relativePosition * 100 << "%";
    
    // If relative position > 99.5%, set to 100%
    if (relativePosition > 0.995) {
        relativePosition = 1.0;
    }
    
    // Save current relative position and update progress bar (thread-safe)
    {
        QMutexLocker locker(&m_progressMutex);
        m_currentRelativePosition = relativePosition;
        int progressValue = static_cast<int>(relativePosition * ui->playProgressbar->maximum() + 0.5);
        ui->playProgressbar->setValue(progressValue);
    }
    // qDebug() << "Set progress bar value to:" << progressValue;
}

void PlayerWG::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    // Update progress bar based on saved relative position when window size changes
    if (m_currentRelativePosition >= 0.0) {
        // Ensure relative position doesn't exceed 100%
        double adjustedPosition = m_currentRelativePosition;
        
        int progressValue = static_cast<int>(adjustedPosition * ui->playProgressbar->maximum() + 0.5);
        ui->playProgressbar->setValue(progressValue);
        // qDebug() << "Window size changed, update progress bar value to:" << progressValue << " (relative position:" << adjustedPosition * 100 << "%)";
    }
    
    if (m_windowEmbedded) {
        QTimer::singleShot(100, this, &PlayerWG::resizeFfplayWindow);
    }
}

void PlayerWG::onMouseEventFromX11(int x_root, int y_root, int windowWidth, int windowHeight, unsigned long windowId)
{
    Q_UNUSED(windowId)

    // Map global/root coordinates to the video widget local coordinates
    QPoint globalPos(x_root, y_root);
    QPoint localPos = ui->videoWidget->mapFromGlobal(globalPos);
    QSize widgetSize = ui->videoWidget->size();

    // Check if the click is inside our video widget
    if (localPos.x() < 0 || localPos.y() < 0 || localPos.x() >= widgetSize.width() || localPos.y() >= widgetSize.height()) {
        // Outside our widget — ignore
        return;
    }

    double relativePosition = 0.0;
    if (widgetSize.width() > 0) {
        relativePosition = static_cast<double>(localPos.x()) / widgetSize.width();
    }

    // Clamp
    if (relativePosition > 1.0) relativePosition = 1.0;
    if (relativePosition < 0.0) relativePosition = 0.0;

    // Save & apply progress under mutex to avoid racing with timer
    int progressValue = 0;
    {
        QMutexLocker locker(&m_progressMutex);
        m_currentRelativePosition = relativePosition;
        progressValue = static_cast<int>(relativePosition * ui->playProgressbar->maximum() + 0.5);
        ui->playProgressbar->setValue(progressValue);
    }

    qDebug() << "PlayerWG: Mouse event from X11 root:" << x_root << y_root
             << "local:" << localPos.x() << localPos.y()
             << "widgetSize:" << widgetSize.width() << "x" << widgetSize.height()
             << "relativePosition:" << relativePosition << "progressValue:" << progressValue;

    // If playing, seek to the clicked position
    if (m_isPlaying && m_ffplayProcess) {
        int seekPercentage = static_cast<int>(relativePosition * 100);

        sendKeyToFfplay("s");

        if (seekPercentage <= 90) {
            sendKeyToFfplay(QString::number(seekPercentage / 10));
        } else {
            sendKeyToFfplay("9");
        }

        qDebug() << "PlayerWG: Seeking to" << seekPercentage << "%";
    }
}

void PlayerWG::onX11KeyEvent(int keySym, unsigned long windowId)
{
    // Handle key events from our embedded ffplay window or global events
    if (windowId == 0 || m_ffplayWindow == 0) return;
    
    // Accept events from our monitored window or global events (when windowId matches our monitored window)
    bool acceptEvent = (windowId == m_ffplayWindow);
    
    // Also accept global space key events - if we have an active ffplay window, assume we're monitoring
    if (!acceptEvent && m_embedHelper && m_ffplayWindow != 0) {
        acceptEvent = true;
    }
    
    if (!acceptEvent) {
        return;
    }

    // Check for space or 'p' (pause) keys
    const int KS_space = XK_space;
    const int KS_p = XK_p;
    const int KS_P = XK_P;

    if (keySym == KS_space || keySym == KS_p || keySym == KS_P) {
        qDebug() << "PlayerWG: Space/pause key detected - keySym:" << keySym << "windowId:" << windowId << "Current state - Playing:" << m_isPlaying << "Paused:" << m_isPaused;
        
        // Directly send 'p' key to ffplay for pause/resume
        sendKeyToFfplay("p");
        sendKeyToFfplay("p");
        // Update UI state to match ffplay state (toggle)
        if (m_isPlaying) {
            m_isPlaying = false;
            m_isPaused = true;
            ui->playPauseBtn->setText("Play");
            m_positionTimer->stop();
            emit stateChanged(false);
        } else {
            m_isPlaying = true;
            m_isPaused = false;
            ui->playPauseBtn->setText("Pause");
            m_positionTimer->start();
            emit stateChanged(true);
        }
    }
}

void PlayerWG::onPositionTimerTimeout()
{
    // Auto-increment progress every second while playing. Use mutex to avoid race with user clicks.
    if (!m_isPlaying) return;

    QMutexLocker locker(&m_progressMutex);
    if (!ui || !ui->playProgressbar) return;

    int maxv = ui->playProgressbar->maximum();
    if (maxv <= 0) return;

    int val = ui->playProgressbar->value();
    if (val < maxv) {
        val++;
        ui->playProgressbar->setValue(val);
        m_currentRelativePosition = static_cast<double>(val) / static_cast<double>(maxv);
    } else {
        // reached end: stop playback timer and update state
        m_positionTimer->stop();
        m_isPlaying = false;
        m_isPaused = false;
        ui->playPauseBtn->setText("Play");
        emit stateChanged(false);
    }
}

void PlayerWG::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    if (m_windowEmbedded) {
        QTimer::singleShot(100, this, &PlayerWG::resizeFfplayWindow);
    }
}
