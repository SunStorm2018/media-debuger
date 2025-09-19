#ifndef PROGRESSDLG_H
#define PROGRESSDLG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QElapsedTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    enum ProgressMode {
        Determinate,    // 确定性进度（有明确最大值）
        Indeterminate,  // 不确定性进度（循环动画）
        Busy            // 繁忙状态（无进度条，只有动画）
    };

    explicit ProgressDialog(QWidget *parent = nullptr,
                            Qt::WindowFlags flags = Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    ~ProgressDialog();

    // 设置进度模式
    void setProgressMode(ProgressMode mode);

    // 设置窗口标题
    void setWindowTitle(const QString &title);

    // 设置信息文本
    void setMessage(const QString &message);

    // 设置进度值（Determinate模式使用）
    void setValue(int value);
    void setRange(int minimum, int maximum);

    // 设置是否显示取消按钮
    void setCancelButtonVisible(bool visible);

    // 设置是否自动关闭
    void setAutoClose(bool autoClose);

    // 获取是否被取消
    bool wasCanceled() const;

    // 重置进度
    void reset();

public slots:
    // 开始进度（Indeterminate/Busy模式）
    void start();

    // 完成进度
    void finish();

    // 取消进度
    void cancel();

signals:
    // 取消信号
    void canceled();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateAnimation();

private:
    void setupUI();
    void updateProgressStyle();

    QProgressBar *m_progressBar;
    QLabel *m_messageLabel;
    QLabel *m_timeLabel;
    QPushButton *m_cancelButton;
    QTimer *m_animationTimer;
    QElapsedTimer *m_elapsedTimer;

    ProgressMode m_mode;
    bool m_autoClose;
    bool m_canceled;
    int m_animationStep;

    QString formatTime(qint64 milliseconds);
};

#endif // PROGRESSDLG_H
