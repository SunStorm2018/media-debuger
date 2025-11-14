// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "mediapropswg.h"
#include "ui_mediapropswg.h"
#include <QVBoxLayout>
#include <QJsonParseError>
#include <QFileInfo>
#include <QDebug>

MediaPropsWG::MediaPropsWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MediaPropsWG)
    , m_formatWidget(nullptr)
    , m_streamsWidget(nullptr)
    , m_framesWidget(nullptr)
    , m_chaptersWidget(nullptr)
{
    ui->setupUi(this);
    setupUI();
    
    // Connect signals
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &MediaPropsWG::onTabChanged);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MediaPropsWG::onRefreshClicked);
}

MediaPropsWG::~MediaPropsWG()
{
    delete ui;
}

void MediaPropsWG::setupUI()
{
    // Create InfoWidgets for each tab
    m_formatWidget = new InfoWidgets(this);
    m_streamsWidget = new InfoWidgets(this);
    m_framesWidget = new InfoWidgets(this);
    m_chaptersWidget = new InfoWidgets(this);
    
    // Add widgets to tabs
    ui->formatLayout->addWidget(m_formatWidget);
    ui->streamsLayout->addWidget(m_streamsWidget);
    ui->framesLayout->addWidget(m_framesWidget);
    ui->chaptersLayout->addWidget(m_chaptersWidget);
    
    // Set search visibility for all widgets
    m_formatWidget->setSearchTitleVisiable(true);
    m_streamsWidget->setSearchTitleVisiable(true);
    m_framesWidget->setSearchTitleVisiable(true);
    m_chaptersWidget->setSearchTitleVisiable(true);
}

void MediaPropsWG::setMediaFile(const QString &fileName)
{
    m_mediaFile = fileName;
    ui->fileLineEdit->setText(fileName);
    loadMediaInfo();
}

void MediaPropsWG::loadMediaInfo()
{
    if (m_mediaFile.isEmpty()) {
        qWarning() << "No media file specified";
        return;
    }
    
    QFileInfo fileInfo(m_mediaFile);
    if (!fileInfo.exists()) {
        qWarning() << "Media file does not exist:" << m_mediaFile;
        return;
    }
    
    // Get complete media info in JSON format
    QString jsonData = m_probe.getMediaInfoJsonFormat(QString("%1 %2 %3").arg(SHOW_FORMAT).arg(SHOW_STREAMS).arg(SHOW_CHAPTERS), m_mediaFile);
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return;
    }
    
    if (!doc.isObject()) {
        qWarning() << "JSON is not an object";
        return;
    }
    
    m_mediaInfo = doc.object();
    
    // Load all tabs
    loadFormatInfo();
    loadStreamsInfo();
    loadFramesInfo();
    loadChaptersInfo();
}

void MediaPropsWG::loadFormatInfo()
{
    if (!m_mediaInfo.contains(FORMAT)) {
        qDebug() << "No format information available";
        return;
    }
    
    QJsonObject formatObj = m_mediaInfo[FORMAT].toObject();
    displayFormatInfo(formatObj);
}

void MediaPropsWG::loadStreamsInfo()
{
    if (!m_mediaInfo.contains(STREAMS)) {
        qDebug() << "No streams information available";
        return;
    }
    
    QJsonArray streamsArray = m_mediaInfo[STREAMS].toArray();
    displayStreamsInfo(streamsArray);
}

void MediaPropsWG::loadFramesInfo()
{
    // Get frames info separately as it can be large
    QString framesData = m_probe.getMediaInfoJsonFormat(SHOW_FRAMES, m_mediaFile);
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(framesData.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Frames JSON parse error:" << parseError.errorString();
        return;
    }
    
    if (!doc.isObject()) {
        qWarning() << "Frames JSON is not an object";
        return;
    }
    
    QJsonObject framesObj = doc.object();
    if (framesObj.contains("frames")) {
        QJsonArray framesArray = framesObj["frames"].toArray();
        displayFramesInfo(framesArray);
    }
}

void MediaPropsWG::loadChaptersInfo()
{
    if (!m_mediaInfo.contains(CHAPTERS)) {
        qDebug() << "No chapters information available";
        return;
    }
    
    QJsonArray chaptersArray = m_mediaInfo[CHAPTERS].toArray();
    displayChaptersInfo(chaptersArray);
}

void MediaPropsWG::displayFormatInfo(const QJsonObject &formatObj)
{
    QStringList headers = getFormatHeaders(formatObj);
    QStringList data = extractFormatData(formatObj, headers);
    
    QList<QStringList> tableData;
    for (int i = 0; i < headers.size(); ++i) {
        tableData.append(QStringList{headers[i], data[i]});
    }
    
    m_formatWidget->init_header_detail_tb(QStringList{"Property", "Value"});
    m_formatWidget->update_data_detail_tb(tableData);
}

void MediaPropsWG::displayStreamsInfo(const QJsonArray &streamsArray)
{
    if (streamsArray.isEmpty()) {
        m_streamsWidget->init_header_detail_tb(QStringList{"No streams available"});
        m_streamsWidget->update_data_detail_tb(QList<QStringList>());
        return;
    }
    
    // Get headers from first stream
    QStringList headers = getStreamHeaders(streamsArray.first().toObject());
    
    QList<QStringList> tableData;
    for (const QJsonValue &streamValue : streamsArray) {
        if (streamValue.isObject()) {
            QJsonObject streamObj = streamValue.toObject();
            QStringList rowData = extractStreamData(streamObj, headers);
            tableData.append(rowData);
        }
    }
    
    m_streamsWidget->init_header_detail_tb(headers);
    m_streamsWidget->update_data_detail_tb(tableData);
}

void MediaPropsWG::displayFramesInfo(const QJsonArray &framesArray)
{
    if (framesArray.isEmpty()) {
        m_framesWidget->init_header_detail_tb(QStringList{"No frames available"});
        m_framesWidget->update_data_detail_tb(QList<QStringList>());
        return;
    }
    
    // Get headers from first frame
    QStringList headers = getFrameHeaders(framesArray.first().toObject());
    
    QList<QStringList> tableData;
    for (const QJsonValue &frameValue : framesArray) {
        if (frameValue.isObject()) {
            QJsonObject frameObj = frameValue.toObject();
            QStringList rowData = extractFrameData(frameObj, headers);
            tableData.append(rowData);
        }
    }
    
    m_framesWidget->init_header_detail_tb(headers);
    m_framesWidget->update_data_detail_tb(tableData);
}

void MediaPropsWG::displayChaptersInfo(const QJsonArray &chaptersArray)
{
    if (chaptersArray.isEmpty()) {
        m_chaptersWidget->init_header_detail_tb(QStringList{"No chapters available"});
        m_chaptersWidget->update_data_detail_tb(QList<QStringList>());
        return;
    }
    
    // Get headers from first chapter
    QStringList headers = getChapterHeaders(chaptersArray.first().toObject());
    
    QList<QStringList> tableData;
    for (const QJsonValue &chapterValue : chaptersArray) {
        if (chapterValue.isObject()) {
            QJsonObject chapterObj = chapterValue.toObject();
            QStringList rowData = extractChapterData(chapterObj, headers);
            tableData.append(rowData);
        }
    }
    
    m_chaptersWidget->init_header_detail_tb(headers);
    m_chaptersWidget->update_data_detail_tb(tableData);
}

QStringList MediaPropsWG::getFormatHeaders(const QJsonObject &formatObj)
{
    QStringList headers;
    for (const QString &key : formatObj.keys()) {
        if (key != TAGS) { // Handle tags separately
            headers.append(key);
        }
    }
    
    // Add common tag fields if tags exist
    if (formatObj.contains(TAGS) && formatObj[TAGS].isObject()) {
        QJsonObject tagsObj = formatObj[TAGS].toObject();
        for (const QString &key : tagsObj.keys()) {
            headers.append(QString("TAG:%1").arg(key));
        }
    }
    
    return headers;
}

QStringList MediaPropsWG::getStreamHeaders(const QJsonObject &streamObj)
{
    QStringList headers;
    for (const QString &key : streamObj.keys()) {
        if (key != TAGS && key != "disposition") { // Handle these separately
            headers.append(key);
        }
    }
    
    // Add common tag fields if tags exist
    if (streamObj.contains(TAGS) && streamObj[TAGS].isObject()) {
        QJsonObject tagsObj = streamObj[TAGS].toObject();
        for (const QString &key : tagsObj.keys()) {
            headers.append(QString("TAG:%1").arg(key));
        }
    }
    
    return headers;
}

QStringList MediaPropsWG::getFrameHeaders(const QJsonObject &frameObj)
{
    QStringList headers;
    for (const QString &key : frameObj.keys()) {
        headers.append(key);
    }
    return headers;
}

QStringList MediaPropsWG::getChapterHeaders(const QJsonObject &chapterObj)
{
    QStringList headers;
    for (const QString &key : chapterObj.keys()) {
        headers.append(key);
    }
    return headers;
}

QStringList MediaPropsWG::extractFormatData(const QJsonObject &formatObj, const QStringList &headers)
{
    QStringList data;
    for (const QString &header : headers) {
        if (header.startsWith("TAG:")) {
            QString tagKey = header.mid(4); // Remove "TAG:" prefix
            if (formatObj.contains(TAGS) && formatObj[TAGS].isObject()) {
                QJsonObject tagsObj = formatObj[TAGS].toObject();
                data.append(tagsObj.value(tagKey).toString());
            } else {
                data.append("");
            }
        } else {
            data.append(formatObj.value(header).toString());
        }
    }
    return data;
}

QStringList MediaPropsWG::extractStreamData(const QJsonObject &streamObj, const QStringList &headers)
{
    QStringList data;
    for (const QString &header : headers) {
        if (header.startsWith("TAG:")) {
            QString tagKey = header.mid(4); // Remove "TAG:" prefix
            if (streamObj.contains(TAGS) && streamObj[TAGS].isObject()) {
                QJsonObject tagsObj = streamObj[TAGS].toObject();
                data.append(tagsObj.value(tagKey).toString());
            } else {
                data.append("");
            }
        } else {
            data.append(streamObj.value(header).toString());
        }
    }
    return data;
}

QStringList MediaPropsWG::extractFrameData(const QJsonObject &frameObj, const QStringList &headers)
{
    QStringList data;
    for (const QString &header : headers) {
        QJsonValue value = frameObj.value(header);
        if (value.isString()) {
            data.append(value.toString());
        } else if (value.isDouble()) {
            data.append(QString::number(value.toDouble()));
        } else if (value.isBool()) {
            data.append(value.toBool() ? "true" : "false");
        } else {
            data.append(value.toString());
        }
    }
    return data;
}

QStringList MediaPropsWG::extractChapterData(const QJsonObject &chapterObj, const QStringList &headers)
{
    QStringList data;
    for (const QString &header : headers) {
        QJsonValue value = chapterObj.value(header);
        if (value.isString()) {
            data.append(value.toString());
        } else if (value.isDouble()) {
            data.append(QString::number(value.toDouble()));
        } else if (value.isBool()) {
            data.append(value.toBool() ? "true" : "false");
        } else {
            data.append(value.toString());
        }
    }
    return data;
}

void MediaPropsWG::onTabChanged(int index)
{
    Q_UNUSED(index);
    // Could be used for lazy loading of tabs
}

void MediaPropsWG::onRefreshClicked()
{
    if (!m_mediaFile.isEmpty()) {
        loadMediaInfo();
    }
}
