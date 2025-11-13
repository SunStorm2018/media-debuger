#ifndef MEDIAPROPSWG_H
#define MEDIAPROPSWG_H

#include <QWidget>
#include <QTabWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <widgets/infotablewg.h>
#include <widgets/basefmtwg.h>
#include <common/zffprobe.h>

// Define missing constants
#define STREAMS "streams"
#define CHAPTERS "chapters"

namespace Ui {
class MediaPropsWG;
}

class MediaPropsWG : public QWidget
{
    Q_OBJECT

public:
    explicit MediaPropsWG(QWidget *parent = nullptr);
    ~MediaPropsWG();

    void setMediaFile(const QString &fileName);
    void loadMediaInfo();

private slots:
    void onTabChanged(int index);
    void onRefreshClicked();

private:
    void setupUI();
    void loadFormatInfo();
    void loadStreamsInfo();
    void loadFramesInfo();
    void loadChaptersInfo();
    
    void displayFormatInfo(const QJsonObject &formatObj);
    void displayStreamsInfo(const QJsonArray &streamsArray);
    void displayFramesInfo(const QJsonArray &framesArray);
    void displayChaptersInfo(const QJsonArray &chaptersArray);
    
    QStringList getStreamHeaders(const QJsonObject &streamObj);
    QStringList getFormatHeaders(const QJsonObject &formatObj);
    QStringList getFrameHeaders(const QJsonObject &frameObj);
    QStringList getChapterHeaders(const QJsonObject &chapterObj);
    
    QStringList extractStreamData(const QJsonObject &streamObj, const QStringList &headers);
    QStringList extractFormatData(const QJsonObject &formatObj, const QStringList &headers);
    QStringList extractFrameData(const QJsonObject &frameObj, const QStringList &headers);
    QStringList extractChapterData(const QJsonObject &chapterObj, const QStringList &headers);

private:
    Ui::MediaPropsWG *ui;
    
    QString m_mediaFile;
    ZFfprobe m_probe;
    
    InfoWidgets *m_formatWidget;
    InfoWidgets *m_streamsWidget;
    InfoWidgets *m_framesWidget;
    InfoWidgets *m_chaptersWidget;
    
    QJsonObject m_mediaInfo;
};

#endif // MEDIAPROPSWG_H