#ifndef JSONFORMATWG_H
#define JSONFORMATWG_H

#include <QWidget>
#include <third_part/QJsonModel/include/QJsonModel.hpp>

namespace Ui {
class JsonFormatWG;
}

class JsonFormatWG : public QWidget
{
    Q_OBJECT

public:
    explicit JsonFormatWG(QWidget *parent = nullptr);
    ~JsonFormatWG();

    bool loadJson(const QByteArray &json);
private:
    Ui::JsonFormatWG *ui;

    QJsonModel * m_model;
};

#endif // JSONFORMATWG_H
