// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#ifndef ZX11EMBEDHELPER_H
#define ZX11EMBEDHELPER_H

#include <QObject>
#include <QString>
#include <QTimer>

#ifdef Q_OS_LINUX
// Forward declaration for X11 types
typedef struct _XDisplay Display;
typedef unsigned long Window;
#endif

enum ZMouseButton {
    ZLeftButton,
    ZMiddleButton,
    ZRightButton
};

class ZX11EmbedHelper : public QObject
{
    Q_OBJECT

public:
    explicit ZX11EmbedHelper(QObject *parent = nullptr);
    ~ZX11EmbedHelper();

    bool initialize();
    void cleanup();
    unsigned long findWindow(const QString &title);
    bool embedWindow(unsigned long child, unsigned long parent);
    bool resizeWindow(unsigned long window, int width, int height);
    bool sendKey(unsigned long window, const QString &key);
    bool showWindow(unsigned long window);
    bool sendMouseClick(unsigned long window, int x, int y, ZMouseButton button);
    bool startEventMonitoring(unsigned long window);
    void stopEventMonitoring();

signals:
    // Emits global (root window) coordinates, the target window width/height and the window id
    void mouseEventReceivedGlobal(int x_root, int y_root, int windowWidth, int windowHeight, unsigned long windowId);
    // Emits when a key is pressed in the monitored window (or globally) with root coords
    void keyEventReceivedGlobal(int keySym, unsigned long windowId);

private:
#ifdef Q_OS_LINUX
    unsigned long findWindowRecursive(Display *display, Window window, const QString &title);
    void checkX11Events();
#endif
    void *m_display;
    unsigned long m_monitoredWindow;
    QTimer *m_eventTimer;
    bool m_monitoringEvents;
    bool m_lastRightButtonDown;
    bool m_spaceKeyHandledByEventQueue; // Track if space key was already handled by event queue
};

#endif // ZX11EMBEDHELPER_H