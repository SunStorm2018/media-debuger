#ifndef ZWINDOWHELPER_H
#define ZWINDOWHELPER_H

#include <QWidget>
#include <QScreen>
#include <QGuiApplication>
#include <QStyle>

class ZWindowHelper
{
public:
    ZWindowHelper();

    // 居中到主屏幕
    static void centerToPrimaryScreen(QWidget* widget, bool dialog = false);

    // 居中到当前屏幕（光标所在屏幕）
    static void centerToCurrentScreen(QWidget* widget, bool dialog = false);

    // 居中到父窗口
    static void centerToParent(QWidget* widget, bool dialog = false);
};

#endif // ZWINDOWHELPER_H
