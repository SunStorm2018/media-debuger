#ifndef ZPROCESS_H
#define ZPROCESS_H

#include <QObject>
#include <QProcess>

class ZProcess : public QObject
{
    Q_OBJECT
public:
    explicit ZProcess(QObject *parent = nullptr);

    ZProcess(const QString &cmd);

signals:

};

#endif // ZPROCESS_H
