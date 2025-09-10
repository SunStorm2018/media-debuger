#ifndef FILESWG_H
#define FILESWG_H

#include <QWidget>
#include "../common/singleton.h"

namespace Ui {
class FilesWG;
}

class FilesWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_SINGLETON(FilesWG)
    explicit FilesWG(QWidget *parent = nullptr);
    ~FilesWG();

private:
    Ui::FilesWG *ui;
};

#endif // FILESWG_H
