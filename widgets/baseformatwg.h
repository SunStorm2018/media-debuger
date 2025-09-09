#ifndef BASEFORMATWG_H
#define BASEFORMATWG_H

#include <QWidget>

class BaseFormatWG : public QWidget
{
    Q_OBJECT
public:
    explicit BaseFormatWG(QWidget *parent = nullptr);

public:
    bool loadData(const QByteArray& jsonData);

signals:

public:
    virtual bool loadJson(const QByteArray &json) = 0;
};

#endif // BASEFORMATWG_H
