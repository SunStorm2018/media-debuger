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
    return getFFprobeCommandOutput(FORMATS);
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

QStringList ZFfprobe::getCodecsFromLibav(int type)
{
    QStringList codecNames;

    const AVCodecDescriptor **codecs;
    unsigned i;
    int nb_codecs = get_codecs_sorted(&codecs);

    for (i = 0; i < nb_codecs; i++) {
        const AVCodecDescriptor *desc = codecs[i];

        if (strstr(desc->name, "_deprecated"))
            continue;

        codecNames.append(desc->name);

        printf("\n");
    }
    av_free(codecs);

    return codecNames;
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
