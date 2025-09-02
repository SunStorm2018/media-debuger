#ifndef ZCODECFLAGPARSER_H
#define ZCODECFLAGPARSER_H
#include <QList>
#include <QString>
#include <QChar>

class ZCodecFlagParser
{
public:
    ZCodecFlagParser();
    static QList<QString> parse(const QString& codecFlags);

    static QString getPositionMeaning(int index);

    static QString getPositionShortName(int index);

private:
    static bool isFlagSet(QChar ch, int position);
};

#endif // ZCODECFLAGPARSER_H
