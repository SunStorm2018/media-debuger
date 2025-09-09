#ifndef TABELFORMATWG_H
#define TABELFORMATWG_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


#include <model/mediainfotabelmodel.h>
#include <widgets/baseformatwg.h>

namespace Ui {
class TabelFormatWG;
}

class TabelFormatWG : public BaseFormatWG
{
    Q_OBJECT

public:
    explicit TabelFormatWG(QWidget *parent = nullptr);
    ~TabelFormatWG();

private:
    Ui::TabelFormatWG *ui;

    QList<QStringList> m_table;
    QList<QString> m_header;
    MediaInfoTabelModel *model;

    QString valueToString(const QJsonValue &value);
    QString extractSideData(const QJsonObject &frameObj, const QString &key);
protected:
    bool loadJson(const QByteArray &json);
};

#endif // TABELFORMATWG_H
