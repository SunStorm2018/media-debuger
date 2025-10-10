#include "zffprobe.h"
#include "qdebug.h"
#include "qrgb.h"

ZFfprobe::ZFfprobe(QObject *parent)
    : QObject{parent}
{}

QString ZFfprobe::getVersion()
{
    if (cacheVersion.isEmpty()) {
        cacheVersion = getFFprobeCommandOutput(VERSION);
    }

    return cacheVersion;
}

QString ZFfprobe::getBuildconf()
{
    return getFFprobeCommandOutput(BUILDCONF);
}

QString ZFfprobe::getFormats()
{
    return getFFprobeCommandOutput(FORMATS);
}

QString ZFfprobe::getMuxers()
{
    return getFFprobeCommandOutput(MUXERS);
}

QString ZFfprobe::getDemuxers()
{
    return getFFprobeCommandOutput(DEMUXERS);
}

QString ZFfprobe::getDevices()
{
    return getFFprobeCommandOutput(DEVICES);
}

QString ZFfprobe::getCodecs()
{
    return getFFprobeCommandOutput(CODECS);
}

QString ZFfprobe::getDecoders()
{
    return getFFprobeCommandOutput(DECODERS);
}

QString ZFfprobe::getEncoders()
{
    return getFFprobeCommandOutput(ENCODERS);
}

QString ZFfprobe::getBsfs()
{
    return getFFprobeCommandOutput(BSFS);
}

QString ZFfprobe::getProtocols()
{
    return getFFprobeCommandOutput(PROTOCOLS);
}

QString ZFfprobe::getFilters()
{
    return getFFprobeCommandOutput(FILTERS);
}

QString ZFfprobe::getPixfmts()
{
    return getFFprobeCommandOutput(PIX_FMTS);
}

QString ZFfprobe::getLayouts()
{
    return getFFprobeCommandOutput(LAYOUTS);
}

QString ZFfprobe::getSamplefmts()
{
    return getFFprobeCommandOutput(SAMPLE_FMTS);
}

QString ZFfprobe::getColors()
{
    return getFFprobeCommandOutput(COLORS);
}

QString ZFfprobe::getL()
{
    return getFFprobeCommandOutput(LICENSE);
}

QString ZFfprobe::getHelp(const QStringList& helpList)
{
    return getFFprobeCommandOutput(HELP, helpList);
}

QString ZFfprobe::getBasicInfo(const QString &function, bool *sucess)
{
    QString retVal;

    QString funWrapper = function;
    if (!funWrapper.startsWith("get")) {
        funWrapper.replace(0, 1, function[0].toUpper());
        funWrapper.prepend("get");
    }

    bool tmp_sucess = QMetaObject::invokeMethod(this, funWrapper.toUtf8(), Qt::DirectConnection,
                                                Q_RETURN_ARG(QString, retVal));
    if (sucess) {
        *sucess = tmp_sucess;
    }

    return retVal;
}

QString ZFfprobe::getMediaInfoJsonFormat(const QString& command, const QString& fileName)
{
    QProcess process;
    process.start(FFPROBE, QStringList() << HIDEBANNER <<
                               LOGLEVEL << QUIET <<
                               OF << JSON <<
                               command.split(" ", QString::SkipEmptyParts)
                                         << fileName);
    qDebug() << process.arguments().join(" ").prepend(" ").prepend(FFPROBE);
    process.waitForFinished();
    return process.readAll();
}

QStringList ZFfprobe::getCodecsFromLibav(CodecType type)
{
    QStringList codecNames;

    const AVCodecDescriptor **codecs;
    unsigned int i;
    int nb_codecs = get_codecs_sorted(&codecs);

    for (i = 0; i < nb_codecs; i++) {
        const AVCodecDescriptor *desc = codecs[i];

        if (type & CODEC_TYPE_DECODER) {
            if (avcodec_find_decoder(desc->id)) {
                codecNames.append(desc->name);
            }
        }

        if (type & CODEC_TYPE_ENCODER) {
            if (avcodec_find_encoder(desc->id)) {
                codecNames.append(desc->name);
            }
        }

        if (strstr(desc->name, "_deprecated"))
            continue;

        printf("\n");
    }
    av_free(codecs);

    return codecNames;
}

QStringList ZFfprobe::getMuxersFromLibav(MuxerType type)
{
    QStringList muxerNames;

    if (type & MUXER_TYPE_MUXER) {
        const AVOutputFormat *fmt = NULL;
        void *opaque = NULL;

        while ((fmt = av_muxer_iterate(&opaque)) != NULL) {
            muxerNames.append(fmt->name);
        }
    }

    if (type & MUXER_TYPE_DEMUXER) {
        const AVInputFormat *fmt = NULL;
        void *opaque = NULL;

        while ((fmt = av_demuxer_iterate(&opaque)) != NULL) {
            muxerNames.append(fmt->name);
        }
    }

    return muxerNames;
}

QStringList ZFfprobe::getFiltersFromLibav()
{
    QStringList filterNames;
    const AVFilter *filter = NULL;
    void *opaque = NULL;

#if LIBAVFILTER_VERSION_INT >= AV_VERSION_INT(7, 14, 100)
    // new api（FFmpeg 4.0+）
    while ((filter = av_filter_iterate(&opaque)) != NULL) {
        if (filter->name && !filterNames.contains(filter->name)) {
            filterNames.append(filter->name);
        }
    }
#else
    filter = av_filter_next(NULL);
    while (filter != NULL) {
        if (filter->name && !filterNames.contains(filter->name)) {
            filterNames.append(filter->name);
        }
        filter = av_filter_next(filter);
    }
#endif

    filterNames.sort();
    return filterNames;
}

QStringList ZFfprobe::getBsfFromLibav()
{
    QStringList bsfNames;
    const AVBitStreamFilter *bsf = NULL;
    void *opaque = NULL;

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(58, 18, 100)
    // new api（FFmpeg 4.0+）
    while ((bsf = av_bsf_iterate(&opaque)) != NULL) {
        if (bsf->name && !bsfNames.contains(bsf->name)) {
            bsfNames.append(bsf->name);
        }
    }
#else
    bsf = av_bsf_next(NULL);
    while (bsf != NULL) {
        if (bsf->name && !bsfNames.contains(bsf->name)) {
            bsfNames.append(bsf->name);
        }
        bsf = av_bsf_next(bsf);
    }
#endif

    bsfNames.sort();
    return bsfNames;
}

QStringList ZFfprobe::getProtocolFromLibav()
{
    QStringList protocolNames;
    void *opaque = NULL;
    const char *protocol_name;

    opaque = NULL;
    while ((protocol_name = avio_enum_protocols(&opaque, 0))) {
        if (protocol_name && !protocolNames.contains(protocol_name)) {
            protocolNames.append(protocol_name);
        }
    }

    opaque = NULL;
    while ((protocol_name = avio_enum_protocols(&opaque, 1))) {
        if (protocol_name && !protocolNames.contains(protocol_name)) {
            protocolNames.append(protocol_name);
        }
    }

    protocolNames.sort();
    return protocolNames;
}

QMap<QString, QList<QVariant>> ZFfprobe::getVideoSize(const QString &key)
{
    static const QMap<QString, QList<QVariant>> videoSizes = {
        {"ntsc", {"720x480", 720, 480}},
        {"pal", {"720x576", 720, 576}},
        {"qntsc", {"352x240", 352, 240}},
        {"qpal", {"352x288", 352, 288}},
        {"sntsc", {"640x480", 640, 480}},
        {"spal", {"768x576", 768, 576}},
        {"film", {"352x240", 352, 240}},
        {"ntsc-film", {"352x240", 352, 240}},
        {"sqcif", {"128x96", 128, 96}},
        {"qcif", {"176x144", 176, 144}},
        {"cif", {"352x288", 352, 288}},
        {"4cif", {"704x576", 704, 576}},
        {"16cif", {"1408x1152", 1408, 1152}},
        {"qqvga", {"160x120", 160, 120}},
        {"qvga", {"320x240", 320, 240}},
        {"vga", {"640x480", 640, 480}},
        {"svga", {"800x600", 800, 600}},
        {"xga", {"1024x768", 1024, 768}},
        {"uxga", {"1600x1200", 1600, 1200}},
        {"qxga", {"2048x1536", 2048, 1536}},
        {"sxga", {"1280x1024", 1280, 1024}},
        {"qsxga", {"2560x2048", 2560, 2048}},
        {"hsxga", {"5120x4096", 5120, 4096}},
        {"wvga", {"852x480", 852, 480}},
        {"wxga", {"1366x768", 1366, 768}},
        {"wsxga", {"1600x1024", 1600, 1024}},
        {"wuxga", {"1920x1200", 1920, 1200}},
        {"woxga", {"2560x1600", 2560, 1600}},
        {"wqsxga", {"3200x2048", 3200, 2048}},
        {"wquxga", {"3840x2400", 3840, 2400}},
        {"whsxga", {"6400x4096", 6400, 4096}},
        {"whuxga", {"7680x4800", 7680, 4800}},
        {"cga", {"320x200", 320, 200}},
        {"ega", {"640x350", 640, 350}},
        {"hd480", {"852x480", 852, 480}},
        {"hd720", {"1280x720", 1280, 720}},
        {"hd1080", {"1920x1080", 1920, 1080}},
        {"2k", {"2048x1080", 2048, 1080}},
        {"2kflat", {"1998x1080", 1998, 1080}},
        {"2kscope", {"2048x858", 2048, 858}},
        {"4k", {"4096x2160", 4096, 2160}},
        {"4kflat", {"3996x2160", 3996, 2160}},
        {"4kscope", {"4096x1716", 4096, 1716}},
        {"nhd", {"640x360", 640, 360}},
        {"hqvga", {"240x160", 240, 160}},
        {"wqvga", {"400x240", 400, 240}},
        {"fwqvga", {"432x240", 432, 240}},
        {"hvga", {"480x320", 480, 320}},
        {"qhd", {"960x540", 960, 540}},
        {"2kdci", {"2048x1080", 2048, 1080}},
        {"4kdci", {"4096x2160", 4096, 2160}},
        {"uhd2160", {"3840x2160", 3840, 2160}},
        {"uhd4320", {"7680x4320", 7680, 4320}}
    };

    QMap<QString, QList<QVariant>> result;

    if (key.isEmpty()) {
        result = videoSizes;
    } else {
        auto it = videoSizes.find(key.toLower());
        if (it != videoSizes.end()) {
            result[it.key()] = it.value();
        }
    }

    return result;
}

QMap<QString, QList<QVariant>> ZFfprobe::getVideoRate(const QString &key)
{
    static const QMap<QString, QList<QVariant>> videoRates = {
        {"ntsc", {"30000/1001", 30000, 1001}},
        {"pal", {"25/1", 25, 1}},
        {"qntsc", {"30000/1001", 30000, 1001}},
        {"qpal", {"25/1", 25, 1}},
        {"sntsc", {"30000/1001", 30000, 1001}},
        {"spal", {"25/1", 25, 1}},
        {"film", {"24/1", 24, 1}},
        {"ntsc-film", {"24000/1001", 24000, 1001}}
    };

    QMap<QString, QList<QVariant>> result;

    if (key.isEmpty()) {
        result = videoRates;
    } else {
        auto it = videoRates.find(key.toLower());
        if (it != videoRates.end()) {
            result[it.key()] = it.value();
        }
    }

    return result;
}

QMap<QString, QList<QVariant>> ZFfprobe::getColor(const QString &key)
{
    static const QMap<QString, QPair<QString, QRgb>> colors = {
        {"AliceBlue", {"0xF0F8FF", 0xF0F8FF}},
        {"AntiqueWhite", {"0xFAEBD7", 0xFAEBD7}},
        {"Aqua", {"0x00FFFF", 0x00FFFF}},
        {"Aquamarine", {"0x7FFFD4", 0x7FFFD4}},
        {"Azure", {"0xF0FFFF", 0xF0FFFF}},
        {"Beige", {"0xF5F5DC", 0xF5F5DC}},
        {"Bisque", {"0xFFE4C4", 0xFFE4C4}},
        {"Black", {"0x000000", 0x000000}},
        {"BlanchedAlmond", {"0xFFEBCD", 0xFFEBCD}},
        {"Blue", {"0x0000FF", 0x0000FF}},
        {"BlueViolet", {"0x8A2BE2", 0x8A2BE2}},
        {"Brown", {"0xA52A2A", 0xA52A2A}},
        {"BurlyWood", {"0xDEB887", 0xDEB887}},
        {"CadetBlue", {"0x5F9EA0", 0x5F9EA0}},
        {"Chartreuse", {"0x7FFF00", 0x7FFF00}},
        {"Chocolate", {"0xD2691E", 0xD2691E}},
        {"Coral", {"0xFF7F50", 0xFF7F50}},
        {"CornflowerBlue", {"0x6495ED", 0x6495ED}},
        {"Cornsilk", {"0xFFF8DC", 0xFFF8DC}},
        {"Crimson", {"0xDC143C", 0xDC143C}},
        {"Cyan", {"0x00FFFF", 0x00FFFF}},
        {"DarkBlue", {"0x00008B", 0x00008B}},
        {"DarkCyan", {"0x008B8B", 0x008B8B}},
        {"DarkGoldenRod", {"0xB8860B", 0xB8860B}},
        {"DarkGray", {"0xA9A9A9", 0xA9A9A9}},
        {"DarkGreen", {"0x006400", 0x006400}},
        {"DarkKhaki", {"0xBDB76B", 0xBDB76B}},
        {"DarkMagenta", {"0x8B008B", 0x8B008B}},
        {"DarkOliveGreen", {"0x556B2F", 0x556B2F}},
        {"Darkorange", {"0xFF8C00", 0xFF8C00}},
        {"DarkOrchid", {"0x9932CC", 0x9932CC}},
        {"DarkRed", {"0x8B0000", 0x8B0000}},
        {"DarkSalmon", {"0xE9967A", 0xE9967A}},
        {"DarkSeaGreen", {"0x8FBC8F", 0x8FBC8F}},
        {"DarkSlateBlue", {"0x483D8B", 0x483D8B}},
        {"DarkSlateGray", {"0x2F4F4F", 0x2F4F4F}},
        {"DarkTurquoise", {"0x00CED1", 0x00CED1}},
        {"DarkViolet", {"0x9400D3", 0x9400D3}},
        {"DeepPink", {"0xFF1493", 0xFF1493}},
        {"DeepSkyBlue", {"0x00BFFF", 0x00BFFF}},
        {"DimGray", {"0x696969", 0x696969}},
        {"DodgerBlue", {"0x1E90FF", 0x1E90FF}},
        {"FireBrick", {"0xB22222", 0xB22222}},
        {"FloralWhite", {"0xFFFAF0", 0xFFFAF0}},
        {"ForestGreen", {"0x228B22", 0x228B22}},
        {"Fuchsia", {"0xFF00FF", 0xFF00FF}},
        {"Gainsboro", {"0xDCDCDC", 0xDCDCDC}},
        {"GhostWhite", {"0xF8F8FF", 0xF8F8FF}},
        {"Gold", {"0xFFD700", 0xFFD700}},
        {"GoldenRod", {"0xDAA520", 0xDAA520}},
        {"Gray", {"0x808080", 0x808080}},
        {"Green", {"0x008000", 0x008000}},
        {"GreenYellow", {"0xADFF2F", 0xADFF2F}},
        {"HoneyDew", {"0xF0FFF0", 0xF0FFF0}},
        {"HotPink", {"0xFF69B4", 0xFF69B4}},
        {"IndianRed", {"0xCD5C5C", 0xCD5C5C}},
        {"Indigo", {"0x4B0082", 0x4B0082}},
        {"Ivory", {"0xFFFFF0", 0xFFFFF0}},
        {"Khaki", {"0xF0E68C", 0xF0E68C}},
        {"Lavender", {"0xE6E6FA", 0xE6E6FA}},
        {"LavenderBlush", {"0xFFF0F5", 0xFFF0F5}},
        {"LawnGreen", {"0x7CFC00", 0x7CFC00}},
        {"LemonChiffon", {"0xFFFACD", 0xFFFACD}},
        {"LightBlue", {"0xADD8E6", 0xADD8E6}},
        {"LightCoral", {"0xF08080", 0xF08080}},
        {"LightCyan", {"0xE0FFFF", 0xE0FFFF}},
        {"LightGoldenRodYellow", {"0xFAFAD2", 0xFAFAD2}},
        {"LightGreen", {"0x90EE90", 0x90EE90}},
        {"LightGrey", {"0xD3D3D3", 0xD3D3D3}},
        {"LightPink", {"0xFFB6C1", 0xFFB6C1}},
        {"LightSalmon", {"0xFFA07A", 0xFFA07A}},
        {"LightSeaGreen", {"0x20B2AA", 0x20B2AA}},
        {"LightSkyBlue", {"0x87CEFA", 0x87CEFA}},
        {"LightSlateGray", {"0x778899", 0x778899}},
        {"LightSteelBlue", {"0xB0C4DE", 0xB0C4DE}},
        {"LightYellow", {"0xFFFFE0", 0xFFFFE0}},
        {"Lime", {"0x00FF00", 0x00FF00}},
        {"LimeGreen", {"0x32CD32", 0x32CD32}},
        {"Linen", {"0xFAF0E6", 0xFAF0E6}},
        {"Magenta", {"0xFF00FF", 0xFF00FF}},
        {"Maroon", {"0x800000", 0x800000}},
        {"MediumAquaMarine", {"0x66CDAA", 0x66CDAA}},
        {"MediumBlue", {"0x0000CD", 0x0000CD}},
        {"MediumOrchid", {"0xBA55D3", 0xBA55D3}},
        {"MediumPurple", {"0x9370D8", 0x9370D8}},
        {"MediumSeaGreen", {"0x3CB371", 0x3CB371}},
        {"MediumSlateBlue", {"0x7B68EE", 0x7B68EE}},
        {"MediumSpringGreen", {"0x00FA9A", 0x00FA9A}},
        {"MediumTurquoise", {"0x48D1CC", 0x48D1CC}},
        {"MediumVioletRed", {"0xC71585", 0xC71585}},
        {"MidnightBlue", {"0x191970", 0x191970}},
        {"MintCream", {"0xF5FFFA", 0xF5FFFA}},
        {"MistyRose", {"0xFFE4E1", 0xFFE4E1}},
        {"Moccasin", {"0xFFE4B5", 0xFFE4B5}},
        {"NavajoWhite", {"0xFFDEAD", 0xFFDEAD}},
        {"Navy", {"0x000080", 0x000080}},
        {"OldLace", {"0xFDF5E6", 0xFDF5E6}},
        {"Olive", {"0x808000", 0x808000}},
        {"OliveDrab", {"0x6B8E23", 0x6B8E23}},
        {"Orange", {"0xFFA500", 0xFFA500}},
        {"OrangeRed", {"0xFF4500", 0xFF4500}},
        {"Orchid", {"0xDA70D6", 0xDA70D6}},
        {"PaleGoldenRod", {"0xEEE8AA", 0xEEE8AA}},
        {"PaleGreen", {"0x98FB98", 0x98FB98}},
        {"PaleTurquoise", {"0xAFEEEE", 0xAFEEEE}},
        {"PaleVioletRed", {"0xD87093", 0xD87093}},
        {"PapayaWhip", {"0xFFEFD5", 0xFFEFD5}},
        {"PeachPuff", {"0xFFDAB9", 0xFFDAB9}},
        {"Peru", {"0xCD853F", 0xCD853F}},
        {"Pink", {"0xFFC0CB", 0xFFC0CB}},
        {"Plum", {"0xDDA0DD", 0xDDA0DD}},
        {"PowderBlue", {"0xB0E0E6", 0xB0E0E6}},
        {"Purple", {"0x800080", 0x800080}},
        {"Red", {"0xFF0000", 0xFF0000}},
        {"RosyBrown", {"0xBC8F8F", 0xBC8F8F}},
        {"RoyalBlue", {"0x4169E1", 0x4169E1}},
        {"SaddleBrown", {"0x8B4513", 0x8B4513}},
        {"Salmon", {"0xFA8072", 0xFA8072}},
        {"SandyBrown", {"0xF4A460", 0xF4A460}},
        {"SeaGreen", {"0x2E8B57", 0x2E8B57}},
        {"SeaShell", {"0xFFF5EE", 0xFFF5EE}},
        {"Sienna", {"0xA0522D", 0xA0522D}},
        {"Silver", {"0xC0C0C0", 0xC0C0C0}},
        {"SkyBlue", {"0x87CEEB", 0x87CEEB}},
        {"SlateBlue", {"0x6A5ACD", 0x6A5ACD}},
        {"SlateGray", {"0x708090", 0x708090}},
        {"Snow", {"0xFFFAFA", 0xFFFAFA}},
        {"SpringGreen", {"0x00FF7F", 0x00FF7F}},
        {"SteelBlue", {"0x4682B4", 0x4682B4}},
        {"Tan", {"0xD2B48C", 0xD2B48C}},
        {"Teal", {"0x008080", 0x008080}},
        {"Thistle", {"0xD8BFD8", 0xD8BFD8}},
        {"Tomato", {"0xFF6347", 0xFF6347}},
        {"Turquoise", {"0x40E0D0", 0x40E0D0}},
        {"Violet", {"0xEE82EE", 0xEE82EE}},
        {"Wheat", {"0xF5DEB3", 0xF5DEB3}},
        {"White", {"0xFFFFFF", 0xFFFFFF}},
        {"WhiteSmoke", {"0xF5F5F5", 0xF5F5F5}},
        {"Yellow", {"0xFFFF00", 0xFFFF00}},
        {"YellowGreen", {"0x9ACD32", 0x9ACD32}}
    };

    QMap<QString, QList<QVariant>> result;

    if (key.isEmpty()) {
        for (auto it = colors.begin(); it != colors.end(); ++it) {
            QList<QVariant> colorInfo;
            colorInfo << it.value().first << it.value().second;
            result[it.key()] = colorInfo;
        }
    } else {
        auto it = colors.find(key);
        if (it != colors.end()) {
            QList<QVariant> colorInfo;
            colorInfo << it.value().first << it.value().second;
            result[it.key()] = colorInfo;
        }
    }

    return result;
}

QString ZFfprobe::getFFprobeCommandOutput(const QString &command, const QStringList &otherParms)
{
    QProcess process;
    process.start(FFPROBE, QStringList() << HIDEBANNER <<
                               LOGLEVEL << QUIET <<
                               command << otherParms);

    qDebug() << "cmd: " << process.arguments().join(" ").prepend(" ").prepend(FFPROBE);
    process.waitForFinished();
    return process.readAll();
}

int ZFfprobe::get_codecs_sorted(const AVCodecDescriptor ***rcodecs)
{
    const AVCodecDescriptor *desc = NULL;
    const AVCodecDescriptor **codecs;
    unsigned nb_codecs = 0, i = 0;

    while ((desc = avcodec_descriptor_next(desc)))
        nb_codecs++;
    if (!(codecs = (const AVCodecDescriptor**)av_calloc(nb_codecs, sizeof(*codecs))))
        return AVERROR(ENOMEM);
    desc = NULL;
    while ((desc = avcodec_descriptor_next(desc)))
        codecs[i++] = desc;

    qsort(codecs, nb_codecs, sizeof(*codecs), compare_codec_desc);
    *rcodecs = codecs;
    return nb_codecs;
}

char ZFfprobe::get_media_type_char(AVMediaType type)
{
    switch (type) {
    case AVMEDIA_TYPE_VIDEO:    return 'V';
    case AVMEDIA_TYPE_AUDIO:    return 'A';
    case AVMEDIA_TYPE_DATA:     return 'D';
    case AVMEDIA_TYPE_SUBTITLE: return 'S';
    case AVMEDIA_TYPE_ATTACHMENT:return 'T';
    default:                    return '?';
    }
}

int ZFfprobe::compare_codec_desc(const void *a, const void *b)
{
    const AVCodecDescriptor * const *da = (const AVCodecDescriptor**)a;
    const AVCodecDescriptor * const *db = (const AVCodecDescriptor**)b;

    return (*da)->type != (*db)->type ? FFDIFFSIGN((*da)->type, (*db)->type) :
               strcmp((*da)->name, (*db)->name);
}
