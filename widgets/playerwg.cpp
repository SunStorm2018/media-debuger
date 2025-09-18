#include "playerwg.h"
#include "ui_playerwg.h"
#include "x11embedhelper.h"
#include <QFileInfo>
#include <QDebug>

PlayerWG::PlayerWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerWG)
    , m_ffplayProcess(nullptr)
    , m_positionTimer(new QTimer(this))
    , m_isPlaying(false)
    , m_isPaused(false)
    , m_duration(0)
    , m_position(0)
    , m_volume(50)
    , m_ffplayWindow(0)
    , m_embedHelper(new X11EmbedHelper(this))
    , m_windowEmbedded(false)
    , m_embedRetryCount(0)
{
    ui->setupUi(this);
    initConnections();

    if (!m_embedHelper->initialize()) {
        qWarning() << "PlayerWG: Failed to initialize X11 embed helper during construction";
    }
    
    ui->positionSlider->setMinimum(0);
    ui->positionSlider->setMaximum(100);
    ui->volumeSlider->setValue(m_volume);
    
    m_positionTimer->setInterval(1000);
    connect(m_positionTimer, &QTimer::timeout, this, &PlayerWG::updatePosition);
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
    connect(ui->positionSlider, &QSlider::valueChanged, this, &PlayerWG::onPositionSliderChanged);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &PlayerWG::onVolumeSliderChanged);
}

void PlayerWG::setMediaFile(const QString &filePath)
{
    m_mediaFile = filePath;
    stop();
    qDebug() << "Media file set to: " + filePath;
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
    m_position = 0;
    ui->positionSlider->setValue(0);
    ui->timeLabel->setText("00:00 / 00:00");
    ui->playPauseBtn->setText("Play");
    m_positionTimer->stop();
    emit stateChanged(false);
}

void PlayerWG::seek(int position)
{
    if (m_ffplayProcess && m_duration > 0) {
        int seekTime = (position * m_duration) / 100;
        sendKeyToFfplay(QString("s"));
        m_position = position;
        emit positionChanged(position);
    }
}

void PlayerWG::setVolume(int volume)
{
    m_volume = volume;
    if (m_ffplayProcess) {
        if (volume > m_volume) {
            for (int i = 0; i < (volume - m_volume) / 5; i++) {
                sendKeyToFfplay("0");
            }
        } else {
            for (int i = 0; i < (m_volume - volume) / 5; i++) {
                sendKeyToFfplay("9");
            }
        }
    }
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

void PlayerWG::onPositionSliderChanged(int value)
{
    if (!ui->positionSlider->isSliderDown()) {
        return;
    }
    seek(value);
}

void PlayerWG::onVolumeSliderChanged(int value)
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

void PlayerWG::updatePosition()
{
    if (!m_isPlaying || m_duration == 0) {
        return;
    }
    
    m_position++;
    if (m_position > m_duration) {
        m_position = m_duration;
        stop();
        return;
    }
    
    ui->positionSlider->setValue((m_position * 100) / m_duration);
    
    int currentMin = m_position / 60;
    int currentSec = m_position % 60;
    int totalMin = m_duration / 60;
    int totalSec = m_duration % 60;
    
    QString timeText = QString("%1:%2 / %3:%4")
                       .arg(currentMin, 2, 10, QChar('0'))
                       .arg(currentSec, 2, 10, QChar('0'))
                       .arg(totalMin, 2, 10, QChar('0'))
                       .arg(totalSec, 2, 10, QChar('0'));
    
    ui->timeLabel->setText(timeText);
    emit positionChanged(m_position);
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

void PlayerWG::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    
    if (m_windowEmbedded) {
        QTimer::singleShot(100, this, &PlayerWG::resizeFfplayWindow);
    }
}

void PlayerWG::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    if (m_windowEmbedded) {
        QTimer::singleShot(100, this, &PlayerWG::resizeFfplayWindow);
    }
}
