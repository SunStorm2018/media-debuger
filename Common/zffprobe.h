#ifndef ZFFPROBE_H
#define ZFFPROBE_H

#include <QObject>
#include <QProcess>

#define FFPROBE "ffprobe"
#define VERSION "-version"
#define BUILDCONF "-buildconf"
#define FORMATS "-formats"
#define MUXERS "-muxers"
#define DEMUXERS "-demuxers"
#define DEVICES "-devices"
#define CODECS "-codecs"
#define DECODERS "-decoders"
#define ENCODERS "-encoders"
#define BSFS "-bsfs"
#define PROTOCOLS "-protocols"
#define FILTERS "-filters"
#define PIX_FMTS "-pix_fmts"
#define LAYOUTS "-layouts"
#define SAMPLE_FMTS "-sample_fmts"
#define COLORS "-colors"
#define HIDEBANNER "-hide_banner"

#define EXECUTE_FFPROBE_COMMAND(command) \
[]() -> QByteArray { \
        QProcess process; \
        process.start(FFPROBE, QStringList() << command << HIDEBANNER); \
        process.waitForFinished(); \
        return process.readAll(); \
}()

class ZFfprobe : public QObject
{
    Q_OBJECT
public:
    explicit ZFfprobe(QObject *parent = nullptr);


    Q_INVOKABLE QString getVersion();               // show version
    Q_INVOKABLE QString getBuildconf();             // show build configuration
    Q_INVOKABLE QString getFormats();               // show available formats
    Q_INVOKABLE QString getMuxers();                // show available muxers
    Q_INVOKABLE QString getDemuxers();              // show available demuxers
    Q_INVOKABLE QString getDevices();               // show available devices
    Q_INVOKABLE QString getCodecs();                // show available codecs
    Q_INVOKABLE QString getDecoders();              // show available decoders
    Q_INVOKABLE QString getEncoders();              // show available encoders
    Q_INVOKABLE QString getBsfs();                  // show available bit stream filters
    Q_INVOKABLE QString getProtocols();             // show available protocols
    Q_INVOKABLE QString getFilters();               // show available filters
    Q_INVOKABLE QString getPixfmts();               // show available pixel formats
    Q_INVOKABLE QString getLayouts();               // show standard channel layouts
    Q_INVOKABLE QString getSamplefmts ();           // show available audio sample formats
    Q_INVOKABLE QString getColors();                // show available color names
signals:

private:
    QString cacheVersion;
};

#endif // ZFFPROBE_H
