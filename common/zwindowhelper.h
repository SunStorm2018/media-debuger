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

    // Center to primary screen
    static void centerToPrimaryScreen(QWidget* widget, bool dialog = false);

    // Center to current screen (where cursor is)
    static void centerToCurrentScreen(QWidget* widget, bool dialog = false);

    // Center to parent window
    static void centerToParent(QWidget* widget, bool dialog = false);
};

#endif // ZWINDOWHELPER_H
