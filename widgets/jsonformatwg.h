#ifndef JSONFORMATWG_H
#define JSONFORMATWG_H

#include <QWidget>

#include <widgets/baseformatwg.h>

#include <third_part/QJsonModel/include/QJsonModel.hpp>

namespace Ui {
class JsonFormatWG;
}

class JsonFormatWG : public BaseFormatWG
{
    Q_OBJECT

public:
    explicit JsonFormatWG(QWidget *parent = nullptr);
    ~JsonFormatWG();

private:
    Ui::JsonFormatWG *ui;

    QJsonModel * m_model;

protected:
    bool loadJson(const QByteArray &json) override;
};

#endif // JSONFORMATWG_H
