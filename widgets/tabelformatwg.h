#ifndef TABELFORMATWG_H
#define TABELFORMATWG_H

#include <QWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

#include <widgets/infowidgets.h>
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
    InfoWidgets *m_tableFormatWg;

    QList<QStringList> m_data_tb;
    QList<QString> m_headers;

    QString valueToString(const QJsonValue &value);
    QString extractSideData(const QJsonObject &frameObj, const QString &key);
protected:
    bool loadJson(const QByteArray &json);
};

#endif // TABELFORMATWG_H
