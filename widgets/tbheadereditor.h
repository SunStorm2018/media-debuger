#ifndef TBHEADEREDITOR_H
#define TBHEADEREDITOR_H

#include <QWidget>

namespace Ui {
class TBHeaderEditor;
}

class TBHeaderEditor : public QWidget
{
    Q_OBJECT

public:
    explicit TBHeaderEditor(QWidget *parent = nullptr);
    ~TBHeaderEditor();

private:
    Ui::TBHeaderEditor *ui;
};

#endif // TBHEADEREDITOR_H
