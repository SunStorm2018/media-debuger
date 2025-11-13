#include "playerwg.h"
#include "ui_playerwg.h"
#include <QFileInfo>
#include <QDebug>

PlayerWG::PlayerWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayerWG)
    , m_ffplayProcess(nullptr)
    , m_positionTimer(new QTimer(this))
    , m_isPlaying(false)
    , m_isPaused(false)
    , m_volume(50)
    , m_ffplayWindow(0)
    , m_embedHelper(new X11EmbedHelper(this))
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
}

void PlayerWG::setMediaFile(const QString &filePath)
{
    m_mediaFile = filePath;
    stop();
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

void PlayerWG::sendMouseToFfplay(const QPoint pos, const MouseButton button)
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
    
    // Save current relative position
    m_currentRelativePosition = relativePosition;
    
    // Set progress bar position with rounding
    int progressValue = static_cast<int>(relativePosition * ui->playProgressbar->maximum() + 0.5);
    ui->playProgressbar->setValue(progressValue);
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

void PlayerWG::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    
    if (m_windowEmbedded) {
        QTimer::singleShot(100, this, &PlayerWG::resizeFfplayWindow);
    }
}
