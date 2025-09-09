#include "baseformatwg.h"

BaseFormatWG::BaseFormatWG(QWidget *parent)
    : QWidget{parent}
{}

bool BaseFormatWG::loadData(const QByteArray &jsonData) {
    return loadJson(jsonData); // 这里可以调用 protected 方法
}
