// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

// CheckBoxSynchronizer.h
#ifndef CHECKBOXSYNCHRONIZER_H
#define CHECKBOXSYNCHRONIZER_H

#include "statesynchronizer.h"
#include <QCheckBox>

class CheckBoxSynchronizer : public StateSynchronizer<QCheckBox>
{
public:
    explicit CheckBoxSynchronizer(QObject *parent = nullptr);
    void addObject(QCheckBox* obj);
};

#endif // CHECKBOXSYNCHRONIZER_H
