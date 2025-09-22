#include "zffprobe.h"
#include "qdebug.h"

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

QString ZFfprobe::getFFprobeCommandOutput(const QString &command, const QStringList &otherParms)
{
    QProcess process;
    process.start(FFPROBE, QStringList() << HIDEBANNER <<
                               LOGLEVEL << QUIET <<
                               command << otherParms);

    qDebug() << process.arguments().join(" ").prepend(" ").prepend(FFPROBE);
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
