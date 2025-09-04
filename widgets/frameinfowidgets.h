#ifndef FRAMEINFOWIDGETS_H
#define FRAMEINFOWIDGETS_H

#include <QWidget>

namespace Ui {
class frameInfowidgets;
}

class frameInfowidgets : public QWidget
{
    Q_OBJECT

public:
    explicit frameInfowidgets(QWidget *parent = nullptr);
    ~frameInfowidgets();

private:
    Ui::frameInfowidgets *ui;
};

#endif // FRAMEINFOWIDGETS_H
