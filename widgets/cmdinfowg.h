#ifndef CMDINFOWG_H
#define CMDINFOWG_H

#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSplitter>
#include <QGroupBox>

#include <common/zffprobe.h>

class CmdInfoWG : public QWidget
{
    Q_OBJECT

public:
    explicit CmdInfoWG(QWidget *parent = nullptr);
    ~CmdInfoWG();

    void displayBasicInfo(const QString &infoType);
    void displayMediaInfo(const QString &filePath);

private slots:
    void onInfoTypeChanged(int index);
    void onRefreshClicked();
    void onSaveClicked();

private:
    void setupUI();
    void setupConnections();
    QString getInfoTypeName(const QString &type);

    ZFfprobe m_ffprobe;

    // UI Components
    QComboBox *m_infoTypeCombo;
    QTextEdit *m_infoDisplay;
    QPushButton *m_refreshBtn;
    QPushButton *m_saveBtn;
    QLabel *m_titleLabel;

    // Info types mapping
    QMap<QString, QString> m_infoTypeMap;
};

#endif // CMDINFOWG_H
