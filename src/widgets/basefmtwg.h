// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef BASEFMTWG_H
#define BASEFMTWG_H

#include <QWidget>
#include "common/common.h"

class BaseFormatWG : public QWidget
{
    Q_OBJECT
public:
    explicit BaseFormatWG(QWidget *parent = nullptr);

public:
    bool loadData(const QByteArray& jsonData);
    void setExtraInfo(const ZExtraInfo &info);
    ZExtraInfo getExtraInfo();

signals:

public:
    virtual bool loadJson(const QByteArray &json) = 0;

private:
    ZExtraInfo m_extraInfo;
};

#endif // BASEFMTWG_H
