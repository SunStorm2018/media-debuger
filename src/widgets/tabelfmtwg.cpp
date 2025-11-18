// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "tabelfmtwg.h"
#include "ui_tabelfmtwg.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QPixmap>
#include <QLabel>
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QDateTime>
#include <QFile>
#include <QScreen>

#include "../common/zffmpeg.h"
#include "../common/zffplay.h"
#include "../common/common.h"

TabelFormatWG::TabelFormatWG(QWidget *parent)
    : BaseFormatWG(parent)
    , ui(new Ui::TabelFormatWG)
{
    ui->setupUi(this);
    m_tableFormatWg = new InfoWidgets(this);
    ui->verticalLayout->addWidget(m_tableFormatWg);

    // Set default window size to 3/5 of screen width and height
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        int width = screenGeometry.width() * 3 / 5;
        int height = screenGeometry.height() * 3 / 5;
        resize(width, height);
    }

    // copy selected text with header action
    m_tableFormatWg->addContextSeparator();
    m_previewImageAction = new QAction("Show Image", m_tableFormatWg);
    connect(m_previewImageAction, &QAction::triggered, this, &TabelFormatWG::previewImage);
    m_tableFormatWg->addContextAction(m_previewImageAction);
}

TabelFormatWG::~TabelFormatWG()
{
    delete ui;
}

bool TabelFormatWG::loadJson(const QByteArray &json)
{
    qDebug() << "here table";

    m_headers.clear();
    m_data_tb.clear();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(json, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    if (!doc.isObject()) {
        qWarning() << "JSON is not an object";
        return false;
    }

    QJsonObject rootObject = doc.object();

    QStringList availableKeys{"frames", "packets"};

    QString key;
    for (auto it : availableKeys) {
        if (!rootObject.contains(it) || !rootObject[it].isArray()) {
            qWarning() << "Missing or invalid " << it << " array";
        } else {
            key = it;
        }
    }

    if (key.isEmpty()) return false;

    QJsonArray framesArray = rootObject[key].toArray();

    if (framesArray.isEmpty()) {
        qDebug() << "Empty frames array";
        return true;
    }

    // Field classification
    struct FieldCategory {
        QStringList common;
        QStringList video;
        QStringList audio;
        QSet<QString> allFields;
    };

    FieldCategory categories = {
        // Common fields
        {
            "media_type", "stream_index", "key_frame", "pkt_pts", "pkt_pts_time",
            "pkt_dts", "pkt_dts_time", "best_effort_timestamp", "best_effort_timestamp_time",
            "pkt_duration", "pkt_duration_time", "pkt_pos", "pkt_size"
        },
        // Video-specific fields
        {
            "width", "height", "pix_fmt", "sample_aspect_ratio", "pict_type",
            "coded_picture_number", "display_picture_number", "interlaced_frame",
            "top_field_first", "repeat_pict", "chroma_location"
        },
        // Audio-specific fields
        {
            "sample_fmt", "nb_samples", "channels", "channel_layout"
        },
        // All fields set
        {}
    };

    // Collect all fields
    for (const QJsonValue &frameValue : framesArray) {
        if (frameValue.isObject()) {
            QJsonObject obj = frameValue.toObject();
            for (const QString &key : obj.keys()) {
                categories.allFields.insert(key);
            }
        }
    }

    // Build optimized column order for each media type
    QHash<QString, QStringList> headerTemplates;

    auto buildHeader = [&](const QStringList& specificFields) {
        QStringList header;
        // Add common fields that exist in data
        for (const QString &field : categories.common) {
            if (categories.allFields.contains(field)) {
                header.append(field);
            }
        }
        // Add specific fields that exist in data
        for (const QString &field : specificFields) {
            if (categories.allFields.contains(field)) {
                header.append(field);
            }
        }
        // Add remaining fields in sorted order
        QSet<QString> otherFields = categories.allFields;
        for (const QString &field : header) {
            otherFields.remove(field);
        }
        QStringList sortedOther = otherFields.values();
        std::sort(sortedOther.begin(), sortedOther.end());
        header.append(sortedOther);
        return header;
    };

    headerTemplates["video"] = buildHeader(categories.video);
    headerTemplates["audio"] = buildHeader(categories.audio);
    headerTemplates["default"] = buildHeader({});

    // Value converter
    auto toString = [](const QJsonValue &val) -> QString {
        if (val.isNull()) return "null";
        if (val.isBool()) return val.toBool() ? "true" : "false";
        if (val.isDouble()) return QString::number(val.toDouble());
        if (val.isString()) return val.toString();
        if (val.isArray()) return QString("[%1 items]").arg(val.toArray().size());
        if (val.isObject()) return "{object}";
        return val.toString();
    };

    // Parse data
    QString currentMediaType;
    for (const QJsonValue &frameValue : framesArray) {
        if (!frameValue.isObject()) continue;

        QJsonObject frameObj = frameValue.toObject();
        QString mediaType = frameObj.contains("media_type") ?
                                frameObj["media_type"].toString() : "default";

        // Dynamically adjust column order
        if (m_headers.isEmpty() || mediaType != currentMediaType) {
            if (headerTemplates.contains(mediaType)) {
                m_headers = headerTemplates[mediaType];
            } else if (headerTemplates.contains("default")) {
                m_headers = headerTemplates["default"];
            } else {
                m_headers = categories.common + categories.allFields.values();
            }
            currentMediaType = mediaType;
        }

        // Extract row data
        QStringList rowData;
        for (const QString &column : m_headers) {
            rowData.append(frameObj.contains(column) ?
                               toString(frameObj[column]) : "");
        }
        m_data_tb.append(rowData);
    }

    qDebug() << "Parsed" << m_data_tb.size() << "frames with" << m_headers.size() << "columns";

    m_tableFormatWg->init_header_detail_tb(m_headers, ", ");
    m_tableFormatWg->update_data_detail_tb(m_data_tb, ", ");

    return true;
}

QString TabelFormatWG::extractSideData(const QJsonObject &frameObj, const QString &key)
{
    if (!frameObj.contains("side_data_list") || !frameObj["side_data_list"].isArray()) {
        return "";
    }

    QJsonArray sideDataArray = frameObj["side_data_list"].toArray();
    QStringList values;

    for (const QJsonValue &sideDataValue : sideDataArray) {
        if (sideDataValue.isObject()) {
            QJsonObject sideDataObj = sideDataValue.toObject();
            if (sideDataObj.contains(key)) {
                values.append(valueToString(sideDataObj[key]));
            }
        }
    }

    return values.join("; ");
}

void TabelFormatWG::previewImage()
{
    // Get the current media file from settings
    Common *common = Common::instance();
    QString currentFile = common->getConfigValue(CURRENTFILE).toString();
    
    if (currentFile.isEmpty()) {
        QMessageBox::warning(this, "Preview Error", "No media file selected for preview.");
        return;
    }

    // Get the image save path from settings
    QString savePath = common->getConfigValue(IMAGE_PREVIEW_PATH_KEY,
                                          QString(DEFAULT_IMAGE_PREVIEW_PATH)).toString();
    
    // Ensure the save directory exists
    QDir saveDir(savePath);
    if (!saveDir.exists()) {
        if (!saveDir.mkpath(".")) {
            QMessageBox::warning(this, "Preview Error",
                             QString("Failed to create preview directory: %1").arg(savePath));
            return;
        }
    }

    // Get selected rows using InfoWidgets::getSelectRows()
    QList<int> selectedRows = m_tableFormatWg->getSelectRows();
    
    // If no rows selected, use the first row
    if (selectedRows.isEmpty()) {
        if (m_data_tb.isEmpty()) {
            QMessageBox::information(this, "Preview Info",
                                 "No frame data available for preview.");
            return;
        }
        selectedRows.append(0);
    }

    // Process each selected row
    for (int selectedRow : selectedRows) {
        if (selectedRow < 0 || selectedRow >= m_data_tb.size()) {
            qWarning() << "Invalid row index:" << selectedRow;
            continue;
        }

        int frameNumber = selectedRow;

        // Generate output filename with timestamp for unique identification
        QFileInfo fileInfo(currentFile);
        QString baseName = fileInfo.baseName();
        
        // Create a subdirectory for the video file
        QDir videoDir(saveDir.absoluteFilePath(baseName));
        if (!videoDir.exists()) {
            if (!videoDir.mkpath(".")) {
                QMessageBox::warning(this, "Preview Error",
                                 QString("Failed to create video directory: %1").arg(videoDir.absolutePath()));
                continue;
            }
        }
        
        QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss_zzz");
        QString outputFileName = QString("%1_frame_%2_%3.jpg").arg(baseName).arg(frameNumber).arg(timestamp);
        QString outputFilePath = videoDir.absoluteFilePath(outputFileName);

        // Extract the frame using ffmpeg
        ZFFmpeg ffmpeg;
        if (!ffmpeg.extractFrame(currentFile, frameNumber, outputFilePath)) {
            QMessageBox::critical(this, "Preview Error",
                              "Failed to extract frame using ffmpeg.\n"
                              "Please ensure ffmpeg is installed and accessible.");
            continue;
        }

        // Verify the extracted image exists
        if (!QFile::exists(outputFilePath)) {
            QMessageBox::warning(this, "Preview Error",
                             "Failed to create the extracted image file.");
            continue;
        }

        // Use zffplay to display the extracted image with size limits
        ZFFplay *ffplay = new ZFFplay(this);
        if (!ffplay->displayImageWithSize(outputFilePath, 800, 600, frameNumber, baseName)) {
            QMessageBox::warning(this, "Preview Error",
                             "Failed to display the extracted image with ffplay.");
            delete ffplay;
            continue;
        }

        // Connect signal to clean up the ffplay instance when playback finishes
        connect(ffplay, &ZFFplay::playbackFinished, [ffplay]() {
            ffplay->deleteLater();
        });
        
        connect(ffplay, &ZFFplay::errorOccurred, [ffplay](const QString &error) {
            qWarning() << "ZFFplay error:" << error;
            ffplay->deleteLater();
        });

        qDebug() << "TabelFormatWG: Extracted frame" << frameNumber
                 << "saved to" << outputFilePath << "and opened with ffplay";
    }
}

QString TabelFormatWG::valueToString(const QJsonValue &value)
{
    if (value.isNull()) return "null";
    if (value.isUndefined()) return "undefined";
    if (value.isBool()) return value.toBool() ? "true" : "false";
    if (value.isDouble()) return QString::number(value.toDouble());
    if (value.isString()) return value.toString();
    if (value.isArray()) return QString("[Array(%1)]").arg(value.toArray().size());
    if (value.isObject()) return "{Object}";
    return value.toString();
}
