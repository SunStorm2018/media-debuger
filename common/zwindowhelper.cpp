#include "zwindowhelper.h"

ZWindowHelper::ZWindowHelper() {}

void ZWindowHelper::centerToPrimaryScreen(QWidget *widget)
{
    if (!widget) return;

    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen) return;

    QRect screenGeometry = screen->availableGeometry();
    QSize windowSize = widget->size();

    int x = (screenGeometry.width() - windowSize.width()) / 2;
    int y = (screenGeometry.height() - windowSize.height()) / 2;

    widget->move(screenGeometry.x() + x, screenGeometry.y() + y);
}

void ZWindowHelper::centerToCurrentScreen(QWidget* widget)
{
    if (!widget) return;

    QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }

    if (!screen) return;

    QRect screenGeometry = screen->availableGeometry();
    QSize windowSize = widget->size();

    int x = screenGeometry.x() + (screenGeometry.width() - windowSize.width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - windowSize.height()) / 2;

    widget->move(x, y);
}

void ZWindowHelper::centerToParent(QWidget* widget)
{
    if (!widget) return;

    QWidget* parent = widget->parentWidget();
    if (!parent) {
        centerToPrimaryScreen(widget);
        return;
    }

    QPoint parentCenter = parent->geometry().center();
    QSize windowSize = widget->size();

    int x = parentCenter.x() - windowSize.width() / 2;
    int y = parentCenter.y() - windowSize.height() / 2;

    widget->move(x, y);
}
