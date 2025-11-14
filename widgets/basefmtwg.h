// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef BASEFMTWG_H
#define BASEFMTWG_H

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

#endif // BASEFMTWG_H
