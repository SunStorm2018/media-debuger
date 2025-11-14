// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

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
