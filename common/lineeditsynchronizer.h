// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

// LineEditSynchronizer.h
#ifndef LINEEDITSYNCHRONIZER_H
#define LINEEDITSYNCHRONIZER_H

#include "statesynchronizer.h"
#include <QLineEdit>

class LineEditSynchronizer : public StateSynchronizer<QLineEdit>
{
public:
    explicit LineEditSynchronizer(QObject *parent = nullptr);
    void addObject(QLineEdit* obj);
};

#endif // LINEEDITSYNCHRONIZER_H
