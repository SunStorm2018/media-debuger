#include "progressdialog.h"
#include <QCloseEvent>
#include <QApplication>
#include <QStyle>
#include <QTime>

ProgressDialog::ProgressDialog(QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , m_mode(Determinate)
    , m_autoClose(true)
    , m_canceled(false)
    , m_animationStep(0)
{
    setupUI();

    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &ProgressDialog::updateAnimation);

    m_elapsedTimer = new QElapsedTimer();

    setWindowTitle(tr("处理中..."));
    setMessage(tr("请稍候..."));
    setCancelButtonVisible(true);
}

ProgressDialog::~ProgressDialog()
{
    delete m_elapsedTimer;
}

void ProgressDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 消息标签
    m_messageLabel = new QLabel(this);
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setWordWrap(true);
    mainLayout->addWidget(m_messageLabel);

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setTextVisible(true);
    mainLayout->addWidget(m_progressBar);

    // 时间标签
    m_timeLabel = new QLabel(this);
    m_timeLabel->setAlignment(Qt::AlignCenter);
    m_timeLabel->setText(tr("已用时间: 00:00:00"));
    mainLayout->addWidget(m_timeLabel);

    // 取消按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_cancelButton = new QPushButton(tr("取消"), this);
    connect(m_cancelButton, &QPushButton::clicked, this, &ProgressDialog::cancel);
    buttonLayout->addWidget(m_cancelButton);

    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    resize(400, 150);
}

void ProgressDialog::setProgressMode(ProgressMode mode)
{
    m_mode = mode;
    updateProgressStyle();
}

void ProgressDialog::setWindowTitle(const QString &title)
{
    QDialog::setWindowTitle(title);
}

void ProgressDialog::setMessage(const QString &message)
{
    m_messageLabel->setText(message);
}

void ProgressDialog::setValue(int value)
{
    if (m_mode == Determinate) {
        m_progressBar->setValue(value);
    }
}

void ProgressDialog::setRange(int minimum, int maximum)
{
    if (m_mode == Determinate) {
        m_progressBar->setRange(minimum, maximum);
    }
}

void ProgressDialog::setCancelButtonVisible(bool visible)
{
    m_cancelButton->setVisible(visible);
}

void ProgressDialog::setAutoClose(bool autoClose)
{
    m_autoClose = autoClose;
}

bool ProgressDialog::wasCanceled() const
{
    return m_canceled;
}

void ProgressDialog::reset()
{
    m_canceled = false;
    m_progressBar->reset();
    m_animationTimer->stop();
    m_elapsedTimer->invalidate();
    m_timeLabel->setText(tr("已用时间: 00:00:00"));
}

void ProgressDialog::start()
{
    reset();
    m_elapsedTimer->start();

    if (m_mode == Indeterminate || m_mode == Busy) {
        m_animationTimer->start(50); // 每50ms更新一次动画
    }

    show();
}

void ProgressDialog::finish()
{
    m_animationTimer->stop();

    if (m_autoClose) {
        accept();
    } else {
        if (m_mode == Determinate) {
            m_progressBar->setValue(m_progressBar->maximum());
        }
    }
}

void ProgressDialog::cancel()
{
    m_canceled = true;
    m_animationTimer->stop();
    emit canceled();
    reject();
}

void ProgressDialog::closeEvent(QCloseEvent *event)
{
    if (m_cancelButton->isVisible()) {
        cancel();
    }
    event->ignore(); // 防止直接关闭，而是通过cancel处理
}

void ProgressDialog::updateAnimation()
{
    if (m_elapsedTimer->isValid()) {
        qint64 elapsed = m_elapsedTimer->elapsed();
        m_timeLabel->setText(tr("已用时间: %1").arg(formatTime(elapsed)));
    }

    if (m_mode == Indeterminate) {
        // 更新不确定进度条的动画
        m_animationStep = (m_animationStep + 1) % 100;
        m_progressBar->setValue(m_animationStep);
    } else if (m_mode == Busy) {
        // 繁忙模式的动画效果
        static const QStringList busyIcons = { "⣾", "⣽", "⣻", "⢿", "⡿", "⣟", "⣯", "⣷" };
        m_animationStep = (m_animationStep + 1) % busyIcons.size();
        QString message = m_messageLabel->text();
        // 移除之前的动画图标
        if (message.startsWith('[') && message.length() > 3) {
            message = message.mid(3).trimmed();
        }
        m_messageLabel->setText(QString("[%1] %2").arg(busyIcons[m_animationStep]).arg(message));
    }
}

void ProgressDialog::updateProgressStyle()
{
    switch (m_mode) {
    case Determinate:
        m_progressBar->setRange(0, 100);
        m_progressBar->setTextVisible(true);
        m_progressBar->setVisible(true);
        break;
    case Indeterminate:
        m_progressBar->setRange(0, 0); // 设置为0表示不确定模式
        m_progressBar->setTextVisible(false);
        m_progressBar->setVisible(true);
        break;
    case Busy:
        m_progressBar->setVisible(false);
        break;
    }
}

QString ProgressDialog::formatTime(qint64 milliseconds)
{
    int seconds = milliseconds / 1000;
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(secs, 2, 10, QLatin1Char('0'));
}
