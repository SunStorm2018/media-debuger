#include "basefmtwg.h"

BaseFormatWG::BaseFormatWG(QWidget *parent)
    : QWidget{parent}
{}

bool BaseFormatWG::loadData(const QByteArray &jsonData) {
    return loadJson(jsonData);
}
