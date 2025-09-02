#include "zffprobe.h"

ZFfprobe::ZFfprobe(QObject *parent)
    : QObject{parent}
{}

QString ZFfprobe::getVersion()
{
    if (cacheVersion.isEmpty()) {
        cacheVersion = EXECUTE_FFPROBE_COMMAND(VERSION);
    }

    return cacheVersion;
}

QString ZFfprobe::getBuildconf()
{
    return EXECUTE_FFPROBE_COMMAND(BUILDCONF);
}

QString ZFfprobe::getFormats()
{
    return EXECUTE_FFPROBE_COMMAND(FORMATS);
}

QString ZFfprobe::getMuxers()
{
    return EXECUTE_FFPROBE_COMMAND(MUXERS);
}

QString ZFfprobe::getDemuxers()
{
    return EXECUTE_FFPROBE_COMMAND(FORMATS);
}

QString ZFfprobe::getDevices()
{
    return EXECUTE_FFPROBE_COMMAND(DEVICES);
}

QString ZFfprobe::getCodecs()
{
    return EXECUTE_FFPROBE_COMMAND(CODECS);
}

QString ZFfprobe::getDecoders()
{
    return EXECUTE_FFPROBE_COMMAND(DECODERS);
}

QString ZFfprobe::getEncoders()
{
    return EXECUTE_FFPROBE_COMMAND(ENCODERS);
}

QString ZFfprobe::getBsfs()
{
    return EXECUTE_FFPROBE_COMMAND(BSFS);
}

QString ZFfprobe::getProtocols()
{
    return EXECUTE_FFPROBE_COMMAND(PROTOCOLS);
}

QString ZFfprobe::getFilters()
{
    return EXECUTE_FFPROBE_COMMAND(FILTERS);
}

QString ZFfprobe::getPixfmts()
{
    return EXECUTE_FFPROBE_COMMAND(PIX_FMTS);
}

QString ZFfprobe::getLayouts()
{
    return EXECUTE_FFPROBE_COMMAND(LAYOUTS);
}

QString ZFfprobe::getSamplefmts()
{
    return EXECUTE_FFPROBE_COMMAND(SAMPLE_FMTS);
}

QString ZFfprobe::getColors()
{
    return EXECUTE_FFPROBE_COMMAND(COLORS);
}
