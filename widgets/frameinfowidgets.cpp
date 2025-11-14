// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "frameinfowidgets.h"
#include "ui_frameinfowidgets.h"

frameInfowidgets::frameInfowidgets(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::frameInfowidgets)
{
    ui->setupUi(this);
}

frameInfowidgets::~frameInfowidgets()
{
    delete ui;
}
