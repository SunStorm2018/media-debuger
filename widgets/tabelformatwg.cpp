#include "tabelformatwg.h"
#include "ui_tabelformatwg.h"

TabelFormatWG::TabelFormatWG(QWidget *parent)
    : BaseFormatWG(parent)
    , ui(new Ui::TabelFormatWG)
{
    ui->setupUi(this);
    m_tableFormatWg = new InfoWidgets(this);
    ui->verticalLayout->addWidget(m_tableFormatWg);
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

    // 字段分类
    struct FieldCategory {
        QStringList common;
        QStringList video;
        QStringList audio;
        QSet<QString> allFields;
    };

    FieldCategory categories = {
        // 通用字段
        {
            "media_type", "stream_index", "key_frame", "pkt_pts", "pkt_pts_time",
            "pkt_dts", "pkt_dts_time", "best_effort_timestamp", "best_effort_timestamp_time",
            "pkt_duration", "pkt_duration_time", "pkt_pos", "pkt_size"
        },
        // 视频特定字段
        {
            "width", "height", "pix_fmt", "sample_aspect_ratio", "pict_type",
            "coded_picture_number", "display_picture_number", "interlaced_frame",
            "top_field_first", "repeat_pict", "chroma_location"
        },
        // 音频特定字段
        {
            "sample_fmt", "nb_samples", "channels", "channel_layout"
        },
        // 所有字段集合
        {}
    };

    // 收集所有字段
    for (const QJsonValue &frameValue : framesArray) {
        if (frameValue.isObject()) {
            QJsonObject obj = frameValue.toObject();
            for (const QString &key : obj.keys()) {
                categories.allFields.insert(key);
            }
        }
    }

    // 构建每种媒体类型的优化列顺序
    QHash<QString, QStringList> headerTemplates;

    auto buildHeader = [&](const QStringList& specificFields) {
        QStringList header = categories.common;
        for (const QString &field : specificFields) {
            if (categories.allFields.contains(field)) {
                header.append(field);
            }
        }
        // 添加其他字段
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

    // 值转换器
    auto toString = [](const QJsonValue &val) -> QString {
        if (val.isNull()) return "null";
        if (val.isBool()) return val.toBool() ? "true" : "false";
        if (val.isDouble()) return QString::number(val.toDouble());
        if (val.isString()) return val.toString();
        if (val.isArray()) return QString("[%1 items]").arg(val.toArray().size());
        if (val.isObject()) return "{object}";
        return val.toString();
    };

    // 解析数据
    QString currentMediaType;
    for (const QJsonValue &frameValue : framesArray) {
        if (!frameValue.isObject()) continue;

        QJsonObject frameObj = frameValue.toObject();
        QString mediaType = frameObj.contains("media_type") ?
                                frameObj["media_type"].toString() : "default";

        // 动态调整列顺序
        if (m_headers.isEmpty() || mediaType != currentMediaType) {
            m_headers = headerTemplates.value(mediaType, headerTemplates["default"]);
            currentMediaType = mediaType;
        }

        // 提取行数据
        QStringList rowData;
        for (const QString &column : m_headers) {
            rowData.append(frameObj.contains(column) ?
                               toString(frameObj[column]) : "");
        }
        m_data_tb.append(rowData);
    }

    qDebug() << "Parsed" << m_data_tb.size() << "frames with" << m_headers.size() << "columns";

    m_tableFormatWg->init_header_detail_tb(m_headers);
    m_tableFormatWg->update_data_detail_tb(m_data_tb);

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
