// SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
// SPDX-License-Identifier: MIT

#include "zx11embedhelper.h"
#include <QFileInfo>
#include <QDebug>
#include <QTime>
#include <cstdlib>  // for getenv
#include <cstring>  // for strlen

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#endif

ZX11EmbedHelper::ZX11EmbedHelper(QObject *parent)
    : QObject(parent)
    , m_display(nullptr)
    , m_monitoredWindow(0)
    , m_eventTimer(new QTimer(this))
    , m_monitoringEvents(false)
    , m_lastRightButtonDown(false)
    , m_spaceKeyHandledByEventQueue(false)
{
    connect(m_eventTimer, &QTimer::timeout, this, &ZX11EmbedHelper::checkX11Events);
}

ZX11EmbedHelper::~ZX11EmbedHelper()
{
    cleanup();
}

bool ZX11EmbedHelper::initialize()
{
#ifdef Q_OS_LINUX
    if (m_display) {
        return true;
    }

    const char* displayEnv = getenv("DISPLAY");
    if (!displayEnv || strlen(displayEnv) == 0) {
        qWarning() << "ZX11EmbedHelper" << "No DISPLAY environment variable set, skipping X11 initialization";
        return false;
    }
    
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
       qWarning() << "ZX11EmbedHelper" << "Failed to open X11 display";
        return false;
    }
    
    qInfo() << "ZX11EmbedHelper" << "X11 display opened successfully";
    return true;
#else
    qWarning() << ("ZX11EmbedHelper", "X11 not supported on this platform");
    return false;
#endif
}

void ZX11EmbedHelper::cleanup()
{
#ifdef Q_OS_LINUX
    if (m_display) {
        XCloseDisplay(static_cast<Display*>(m_display));
        m_display = nullptr;
    }
#endif
}

unsigned long ZX11EmbedHelper::findWindow(const QString &title)
{
#ifdef Q_OS_LINUX
    if (!m_display) {
        return 0;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window root = DefaultRootWindow(display);

    return findWindowRecursive(display, root, title);
#else
    Q_UNUSED(title)
    return 0;
#endif
}

#ifdef Q_OS_LINUX
unsigned long ZX11EmbedHelper::findWindowRecursive(Display *display, Window window, const QString &title)
{
    char *window_name = nullptr;
    if (XFetchName(display, window, &window_name) && window_name) {
        QString name(window_name);

        bool matches = false;
        if (!title.isEmpty()) {
            matches = (name == title || name.contains(title, Qt::CaseSensitive));
        }

        if (!matches) {
            matches = (name.startsWith("ffplay", Qt::CaseInsensitive) && 
                      (name.contains(".mp4") || name.contains(".mkv") || 
                       name.contains(".avi") || name.contains(".mov") ||
                       name.contains(".wmv") || name.contains(".flv") ||
                       name.contains(".m4v") || name.contains(".webm")));
        }
        
        if (matches) {
            qDebug() << "ZX11EmbedHelper: Found target window:" << name << "(window ID:" << window << ")";
            XFree(window_name);
            return static_cast<unsigned long>(window);
        }
        XFree(window_name);
    }

    Window parent, *children;
    unsigned int nchildren;
    
    if (XQueryTree(display, window, &window, &parent, &children, &nchildren) != 0) {
        for (unsigned int i = 0; i < nchildren; i++) {
            unsigned long result = findWindowRecursive(display, children[i], title);
            if (result != 0) {
                XFree(children);
                return result;
            }
        }
        XFree(children);
    }
    
    return 0;
}
#endif

bool ZX11EmbedHelper::embedWindow(unsigned long child, unsigned long parent)
{
#ifdef Q_OS_LINUX
    if (!m_display || child == 0 || parent == 0) {
        return false;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window childWindow = static_cast<Window>(child);
    Window parentWindow = static_cast<Window>(parent);

    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, childWindow, &attrs) == 0) {
        qWarning() << "ZX11EmbedHelper: Child window does not exist";
        return false;
    }

    XUnmapWindow(display, childWindow);
    XFlush(display);

    XReparentWindow(display, childWindow, parentWindow, 0, 0);
    XFlush(display);

    XSetWindowBorderWidth(display, childWindow, 0);

    XMapWindow(display, childWindow);
    XFlush(display);
    
    qInfo() << "ZX11EmbedHelper: Window embedded successfully";
    return true;
#else
    Q_UNUSED(child)
    Q_UNUSED(parent)
    return false;
#endif
}

bool ZX11EmbedHelper::resizeWindow(unsigned long window, int width, int height)
{
#ifdef Q_OS_LINUX
    if (!m_display || window == 0) {
        return false;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window win = static_cast<Window>(window);
    
    XResizeWindow(display, win, width, height);
    XFlush(display);
    return true;
#else
    Q_UNUSED(window)
    Q_UNUSED(width)
    Q_UNUSED(height)
    return false;
#endif
}

bool ZX11EmbedHelper::sendKey(unsigned long window, const QString &key)
{
#ifdef Q_OS_LINUX
    if (!m_display || window == 0) {
        return false;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window win = static_cast<Window>(window);
    
    KeySym keysym = XStringToKeysym(key.toLatin1().data());
    if (keysym == NoSymbol) {
        return false;
    }
    
    KeyCode keycode = XKeysymToKeycode(display, keysym);
    
    XKeyEvent event;
    event.display = display;
    event.window = win;
    event.root = DefaultRootWindow(display);
    event.subwindow = None;
    event.time = CurrentTime;
    event.x = 1;
    event.y = 1;
    event.x_root = 1;
    event.y_root = 1;
    event.same_screen = True;
    event.keycode = keycode;
    event.state = 0;
    
    event.type = KeyPress;
    XSendEvent(display, win, True, KeyPressMask, (XEvent*)&event);
    
    event.type = KeyRelease;
    XSendEvent(display, win, True, KeyReleaseMask, (XEvent*)&event);
    
    XFlush(display);
    return true;
#else
    Q_UNUSED(window)
    Q_UNUSED(key)
    return false;
#endif
}

bool ZX11EmbedHelper::showWindow(unsigned long window)
{
#ifdef Q_OS_LINUX
    if (!m_display || window == 0) {
        return false;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window win = static_cast<Window>(window);
    
    XMapWindow(display, win);
    XRaiseWindow(display, win);
    XFlush(display);
    return true;
#else
    Q_UNUSED(window)
    return false;
#endif
}

bool ZX11EmbedHelper::sendMouseClick(unsigned long window, int x, int y, ZMouseButton button)
{
#ifdef Q_OS_LINUX
    if (!m_display || window == 0) {
        return false;
    }

    Display *display = static_cast<Display*>(m_display);
    Window win = static_cast<Window>(window);

    unsigned int x11Button;
    unsigned int x11StateMask;

    switch (button) {
    case ZLeftButton:
        x11Button = Button1;
        x11StateMask = Button1Mask;
        break;
    case ZMiddleButton:
        x11Button = Button2;
        x11StateMask = Button2Mask;
        break;
    case ZRightButton:
        x11Button = Button3;
        x11StateMask = Button3Mask;
        break;
    default:
        return false;
    }

    XButtonEvent pressEvent;
    pressEvent.display = display;
    pressEvent.window = win;
    pressEvent.root = DefaultRootWindow(display);
    pressEvent.subwindow = None;
    pressEvent.time = CurrentTime;
    pressEvent.x = x;
    pressEvent.y = y;
    pressEvent.x_root = 1;
    pressEvent.y_root = 1;
    pressEvent.same_screen = True;
    pressEvent.button = x11Button;
    pressEvent.state = 0;

    pressEvent.type = ButtonPress;
    XSendEvent(display, win, True, ButtonPressMask, (XEvent*)&pressEvent);

    XButtonEvent releaseEvent = pressEvent;
    releaseEvent.type = ButtonRelease;
    releaseEvent.state = x11StateMask;

    XSendEvent(display, win, True, ButtonReleaseMask, (XEvent*)&releaseEvent);

    XFlush(display);
    return true;
#else
    Q_UNUSED(window)
    Q_UNUSED(x)
    Q_UNUSED(y)
    Q_UNUSED(button)
    return false;
#endif
}

bool ZX11EmbedHelper::startEventMonitoring(unsigned long window)
{
#ifdef Q_OS_LINUX
    if (!m_display || window == 0) {
        return false;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window win = static_cast<Window>(window);
    
    // Select input mask for mouse button and key events (without grabbing)
    // This allows us to listen to events without intercepting them
    long eventMask = ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask;
    XSelectInput(display, win, eventMask);
    // Also listen on root window so we receive global clicks/keys (useful if events go to child windows)
    Window root = DefaultRootWindow(display);
    XSelectInput(display, root, eventMask);
    XFlush(display);
    
    m_monitoredWindow = window;
    m_monitoringEvents = true;
    
    // Start the event timer to check for events periodically
    m_eventTimer->start(50); // Check every 50ms
    
    qDebug() << "ZX11EmbedHelper: Started event monitoring for window" << window;
    return true;
#else
    Q_UNUSED(window)
    return false;
#endif
}

void ZX11EmbedHelper::stopEventMonitoring()
{
#ifdef Q_OS_LINUX
    if (m_display && m_monitoringEvents) {
        Display *display = static_cast<Display*>(m_display);
        XUngrabPointer(display, CurrentTime);
        // Remove our root selection so we stop receiving global button events
        Window root = DefaultRootWindow(display);
        XSelectInput(display, root, 0);
        XFlush(display);
    }
#endif
    
    m_eventTimer->stop();
    m_monitoringEvents = false;
    m_monitoredWindow = 0;
    qDebug() << "ZX11EmbedHelper: Stopped event monitoring";
}

#ifdef Q_OS_LINUX
void ZX11EmbedHelper::checkX11Events()
{
    if (!m_display || !m_monitoringEvents || m_monitoredWindow == 0) {
        return;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window win = static_cast<Window>(m_monitoredWindow);
    
    // Check for pending events
    int pending = XPending(display);
    while (pending > 0) {
        XEvent event;
        XNextEvent(display, &event);
        
        // Process events that occur on the monitored window or any of its subwindows.
        Window evwin = event.xany.window;
        bool isForMonitored = false;
        Window cur = evwin;

        if (cur == win) {
            isForMonitored = true;
        } else {
            // Walk up the parent chain to see if monitored window is an ancestor
            while (cur != None && cur != 0 && !isForMonitored) {
                Window root_return, parent_return, *children_return;
                unsigned int nchildren_return;
                if (XQueryTree(display, cur, &root_return, &parent_return, &children_return, &nchildren_return) == 0) {
                    break;
                }
                if (children_return) XFree(children_return);
                if (parent_return == win) {
                    isForMonitored = true;
                    break;
                }
                // move up
                if (parent_return == None || parent_return == 0) break;
                cur = parent_return;
            }
        }

        if (isForMonitored) {
            if (event.type == ButtonPress) {
                XButtonEvent *buttonEvent = reinterpret_cast<XButtonEvent*>(&event);

                // Check if it's a right button click (Button3)
                if (buttonEvent->button == Button3) {
                    // Get window attributes to get monitored window dimensions
                    XWindowAttributes attrs;
                    if (XGetWindowAttributes(display, win, &attrs)) {
                        // Translate event coordinates to monitored window local coords (for debug)
                        int tx = 0, ty = 0;
                        Window child_return = None;
                        XTranslateCoordinates(display, evwin, win, buttonEvent->x, buttonEvent->y, &tx, &ty, &child_return);

                        qDebug() << "ZX11EmbedHelper: Right button click detected at"
                                 << "event-window-local:" << buttonEvent->x << "," << buttonEvent->y
                                 << "translated-to-monitored:" << tx << "," << ty
                                 << "root:" << buttonEvent->x_root << "," << buttonEvent->y_root
                                 << "window size:" << attrs.width << "x" << attrs.height;

                        // Emit signal with root (global) coordinates, monitored window size and window id
                        emit mouseEventReceivedGlobal(buttonEvent->x_root, buttonEvent->y_root,
                                                      attrs.width, attrs.height, static_cast<unsigned long>(win));
                    }
                }
            }
        }
        
        pending = XPending(display);
    }

    // If there were no pending events (or after processing them), do a polling fallback using XQueryPointer.
    // This helps in environments where other clients grab the pointer and prevent events from being queued.
    Window root_win = DefaultRootWindow(display);
    Window child_return = None;
    int root_x = 0, root_y = 0, win_x = 0, win_y = 0;
    unsigned int mask_return = 0;

    if (XQueryPointer(display, root_win, &root_win, &child_return, &root_x, &root_y, &win_x, &win_y, &mask_return)) {
        bool rightDown = (mask_return & Button3Mask);

        if (rightDown && !m_lastRightButtonDown) {
            // Right button just pressed — check whether pointer is over monitored window
            int tx = 0, ty = 0;
            Window trans_child = None;
            if (XTranslateCoordinates(display, DefaultRootWindow(display), win, root_x, root_y, &tx, &ty, &trans_child)) {
                XWindowAttributes attrs;
                if (XGetWindowAttributes(display, win, &attrs)) {
                    if (tx >= 0 && ty >= 0 && tx < attrs.width && ty < attrs.height) {
                        qDebug() << "ZX11EmbedHelper: Poll detected right button press at root:" << root_x << root_y
                                 << "translated to monitored:" << tx << ty << "window size:" << attrs.width << "x" << attrs.height;

                        emit mouseEventReceivedGlobal(root_x, root_y, attrs.width, attrs.height, static_cast<unsigned long>(win));
                    }
                }
            }
        }

        m_lastRightButtonDown = rightDown;
    }

    // Check for global keyboard events by polling the root window
    // This helps capture space key events even when they're not directly sent to our monitored window
    if (m_monitoringEvents && m_monitoredWindow != 0) {
        char keys_return[32];
        XQueryKeymap(display, keys_return);
        
        // Check space key (keycode 65 typically for space on most keyboards)
        const int space_keycode = 65; // Common keycode for space key
        int byte_index = space_keycode / 8;
        int bit_index = space_keycode % 8;
        
        if (byte_index < 32) {
            bool space_pressed = (keys_return[byte_index] & (1 << bit_index)) != 0;
            
            static bool last_space_pressed = false;
            static QTime last_space_time;
            
            if (space_pressed && !last_space_pressed) {
                // Space key just pressed - add simple debounce
                QTime currentTime = QTime::currentTime();
                 if (last_space_time.isNull() || last_space_time.msecsTo(currentTime) > 200) {
                    KeySym keysym = XStringToKeysym("space");
                    if (keysym != NoSymbol) {
                        qDebug() << "ZX11EmbedHelper: Global space key detected via polling";
                        emit keyEventReceivedGlobal(static_cast<int>(keysym), m_monitoredWindow);
                        last_space_time = currentTime;
                    }
                }
            }
            
            last_space_pressed = space_pressed;
        }
    }
}
#endif