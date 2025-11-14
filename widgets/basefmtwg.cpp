// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "basefmtwg.h"

BaseFormatWG::BaseFormatWG(QWidget *parent)
    : QWidget{parent}
{}

bool BaseFormatWG::loadData(const QByteArray &jsonData) {
    return loadJson(jsonData);
}
