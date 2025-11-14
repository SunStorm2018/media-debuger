// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

// SpinBoxSynchronizer.h
#ifndef SPINBOXSYNCHRONIZER_H
#define SPINBOXSYNCHRONIZER_H

#include "statesynchronizer.h"
#include <QSpinBox>

class SpinBoxSynchronizer : public StateSynchronizer<QSpinBox>
{
public:
    explicit SpinBoxSynchronizer(QObject *parent = nullptr);
    void addObject(QSpinBox* obj);
};

#endif // SPINBOXSYNCHRONIZER_H
