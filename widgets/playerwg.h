#ifndef PLAYERWG_H
#define PLAYERWG_H

#include <QWidget>
#include <common/singleton.h>

namespace Ui {
class PlayerWG;
}

class PlayerWG : public QWidget
{
    Q_OBJECT

public:
    DECLARE_SINGLETON(PlayerWG)
    explicit PlayerWG(QWidget *parent = nullptr);
    ~PlayerWG();

private:
    Ui::PlayerWG *ui;
};

#endif // PLAYERWG_H
