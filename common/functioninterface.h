#ifndef FUNCTIONINTERFACE_H
#define FUNCTIONINTERFACE_H

#include <QObject>

class FunctionInterface : public QObject
{
    Q_OBJECT
public:
    explicit FunctionInterface(QObject *parent = nullptr);

signals:
};

#endif // FUNCTIONINTERFACE_H
