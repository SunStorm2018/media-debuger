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
