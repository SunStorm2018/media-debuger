#include "zcodecflagparser.h"

ZCodecFlagParser::ZCodecFlagParser() {}

QList<QString> ZCodecFlagParser::parse(const QString &codecFlags)
{
    QList<QString> result;
    const int totalPositions = 8;

    for (int i = 0; i < totalPositions; ++i) {
        if (i < codecFlags.length()) {
            result.append(isFlagSet(codecFlags[i], i) ? "√" : "");
        } else {
            result.append("");
        }
    }

    return result;
}

QString ZCodecFlagParser::getPositionMeaning(int index)
{
    static const QStringList meanings = {
        "Decoding supported",
        "Encoding supported",
        "Video codec",
        "Audio codec",
        "Subtitle codec",      // First S: subtitle codec
        "Intra frame-only codec",
        "Lossy compression",
        "Lossless compression" // Second S: lossless compression
    };

    return (index >= 0 && index < meanings.size()) ? meanings[index] : "";
}

QString ZCodecFlagParser::getPositionShortName(int index)
{
    static const QStringList shortNames = {"D", "E", "V", "A", "S", "I", "L", "S"};
    return (index >= 0 && index < shortNames.size()) ? shortNames[index] : "";
}

bool ZCodecFlagParser::isFlagSet(QChar ch, int position)
{
    static const QVector<QChar> expectedChars = {'D', 'E', 'V', 'A', 'S', 'I', 'L', 'S'};

    if (position >= 0 && position < expectedChars.size()) {
        return ch == expectedChars[position];
    }
    return false;
}
