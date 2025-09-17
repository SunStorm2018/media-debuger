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
    
    // 现在可以安全地初始化X11了
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
    qDebug() << "PlayerWG", "Media file set to: " + filePath;
}

void PlayerWG::play()
{
    if (m_mediaFile.isEmpty()) {
        qWarning() << "PlayerWG" << "No media file set";
        return;
    }

    // X11已经在构造函数中初始化了
    if (!m_embedHelper->initialize()) {
        qCritical() << "PlayerWG" << "X11 embed helper not available";
        // 即使X11初始化失败，也允许继续播放（只是不能嵌入窗口）
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
    
    qDebug() << "PlayerWG" << "FFplay process finished";
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
    
    // 尝试设置窗口标题，如果失败则使用默认
    QString windowTitle = QString("ffplay_%1").arg(QFileInfo(m_mediaFile).baseName());
    arguments << "-window_title" << windowTitle;
    
    // 如果是嵌入模式，添加相关选项
    if (m_embedHelper && m_embedHelper->initialize()) {
        arguments << "-noborder";
        // 设置初始窗口大小，避免最大化
        QSize videoSize = ui->videoWidget->size();
        if (videoSize.width() > 100 && videoSize.height() > 100) {
            arguments << "-x" << QString::number(videoSize.width());
            arguments << "-y" << QString::number(videoSize.height());
        }
        // 添加隐藏窗口选项，先不显示窗口
        // 这样可以避免用户看到独立窗口闪烁
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
    
    // 更短的延迟，并且立即开始尝试嵌入
    QTimer::singleShot(500, this, &PlayerWG::embedFfplayWindow);
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
    m_embedRetryCount = 0;  // 重置重试计数器
}

void PlayerWG::embedFfplayWindow()
{
    if (!m_ffplayProcess) {
        return;
    }
    
    // 使用实例变量而非静态变量，避免多个实例间干扰
    if (m_embedRetryCount >= 20) {  // 增加重试次数，但缩短间隔
        qCritical() << "PlayerWG: Failed to find ffplay window after 20 retries. FFplay is running but not embedded.";
        m_embedRetryCount = 0;  // 重置计数器
        return;
    }
    
    // 更精确的窗口标题匹配逻辑
    QString baseTitle = QString("ffplay_%1").arg(QFileInfo(m_mediaFile).baseName());
    QString fileName = QFileInfo(m_mediaFile).fileName();
    
    if (m_embedRetryCount == 0) {
        qDebug() << "PlayerWG: Starting window search for ffplay embedding";
        qDebug() << "PlayerWG: Looking for window titles:" << baseTitle << "or containing" << fileName;
    }
    
    // 首先尝试精确匹配指定的窗口标题
    m_ffplayWindow = m_embedHelper->findWindow(baseTitle);
    
    // 如果没有找到，尝试文件名匹配
    if (m_ffplayWindow == 0) {
        m_ffplayWindow = m_embedHelper->findWindow(fileName);
    }
    
    // 最后尝试一般性的ffplay匹配
    if (m_ffplayWindow == 0) {
        m_ffplayWindow = m_embedHelper->findWindow("ffplay");
    }
    
    if (m_ffplayWindow == 0) {
        m_embedRetryCount++;
        // 更短的重试间隔，更快速的嵌入
        QTimer::singleShot(250, this, &PlayerWG::embedFfplayWindow);
        return;
    }
    
    // 找到窗口，重置重试计数器
    m_embedRetryCount = 0;
    
    unsigned long parentWindow = ui->videoWidget->winId();
    qInfo() << "PlayerWG: Found ffplay window" << m_ffplayWindow << ", attempting to embed into parent" << parentWindow;
    
    if (m_embedHelper->embedWindow(m_ffplayWindow, parentWindow)) {
        m_windowEmbedded = true;
        // 立即调整窗口大小
        resizeFfplayWindow();
        qInfo() << "PlayerWG: FFplay window embedded successfully";
        
        // 显示嵌入的窗口
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
        qWarning() << "PlayerWG" << "Failed to send key to FFplay: " + key;
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
