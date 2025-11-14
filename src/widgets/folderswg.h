// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef FOLDERSWG_H
#define FOLDERSWG_H

#include <QWidget>

#include <common/singleton.h>

namespace Ui {
class FoldersWG;
}

class FoldersWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_SINGLETON(FoldersWG)
    explicit FoldersWG(QWidget *parent = nullptr);
    ~FoldersWG();

private:
    Ui::FoldersWG *ui;
};

#endif // FOLDERSWG_H
