// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "folderswg.h"
#include "ui_folderswg.h"

FoldersWG::FoldersWG(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FoldersWG)
{
    ui->setupUi(this);
}

FoldersWG::~FoldersWG()
{
    delete ui;
}
