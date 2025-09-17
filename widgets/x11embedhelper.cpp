#include "x11embedhelper.h"
#include <QFileInfo>
#include <QDebug>
#include <cstdlib>  // for getenv
#include <cstring>  // for strlen

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#include <X11/keysym.h>
#endif

X11EmbedHelper::X11EmbedHelper(QObject *parent)
    : QObject(parent)
    , m_display(nullptr)
{
}

X11EmbedHelper::~X11EmbedHelper()
{
    cleanup();
}

bool X11EmbedHelper::initialize()
{
#ifdef Q_OS_LINUX
    if (m_display) {
        return true;
    }
    
    // 检查DISPLAY环境变量，如果没有设置则跳过X11初始化
    const char* displayEnv = getenv("DISPLAY");
    if (!displayEnv || strlen(displayEnv) == 0) {
        qWarning() << "X11EmbedHelper" << "No DISPLAY environment variable set, skipping X11 initialization";
        return false;
    }
    
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
       qWarning() << "X11EmbedHelper" << "Failed to open X11 display";
        return false;
    }
    
    qInfo() << "X11EmbedHelper" << "X11 display opened successfully";
    return true;
#else
    qWarning() << ("X11EmbedHelper", "X11 not supported on this platform");
    return false;
#endif
}

void X11EmbedHelper::cleanup()
{
#ifdef Q_OS_LINUX
    if (m_display) {
        XCloseDisplay(static_cast<Display*>(m_display));
        m_display = nullptr;
    }
#endif
}

unsigned long X11EmbedHelper::findWindow(const QString &title)
{
#ifdef Q_OS_LINUX
    if (!m_display) {
        return 0;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window root = DefaultRootWindow(display);
    
    // 递归查找所有窗口，包括子窗口
    return findWindowRecursive(display, root, title);
#else
    Q_UNUSED(title)
    return 0;
#endif
}

#ifdef Q_OS_LINUX
unsigned long X11EmbedHelper::findWindowRecursive(Display *display, Window window, const QString &title)
{
    // 检查当前窗口
    char *window_name = nullptr;
    if (XFetchName(display, window, &window_name) && window_name) {
        QString name(window_name);
        
        // 更精确的窗口匹配逻辑
        bool matches = false;
        if (!title.isEmpty()) {
            // 精确匹配指定标题
            matches = (name == title || name.contains(title, Qt::CaseSensitive));
        }
        
        // 如果没有找到精确匹配，再尝试ffplay相关匹配
        if (!matches) {
            matches = (name.startsWith("ffplay", Qt::CaseInsensitive) && 
                      (name.contains(".mp4") || name.contains(".mkv") || 
                       name.contains(".avi") || name.contains(".mov") ||
                       name.contains(".wmv") || name.contains(".flv")));
        }
        
        if (matches) {
            qDebug() << "X11EmbedHelper: Found target window:" << name << "(window ID:" << window << ")";
            XFree(window_name);
            return static_cast<unsigned long>(window);
        }
        XFree(window_name);
    }
    
    // 递归查找子窗口
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

bool X11EmbedHelper::embedWindow(unsigned long child, unsigned long parent)
{
#ifdef Q_OS_LINUX
    if (!m_display || child == 0 || parent == 0) {
        return false;
    }
    
    Display *display = static_cast<Display*>(m_display);
    Window childWindow = static_cast<Window>(child);
    Window parentWindow = static_cast<Window>(parent);
    
    // 检查窗口是否存在
    XWindowAttributes attrs;
    if (XGetWindowAttributes(display, childWindow, &attrs) == 0) {
        qWarning() << "X11EmbedHelper: Child window does not exist";
        return false;
    }
    
    // 首先将子窗口取消映射，避免闪烁
    XUnmapWindow(display, childWindow);
    XFlush(display);
    
    // 重新父化窗口
    XReparentWindow(display, childWindow, parentWindow, 0, 0);
    XFlush(display);
    
    // 设置窗口属性，禁止最大化和移动
    XSetWindowBorderWidth(display, childWindow, 0);
    
    // 重新映射窗口
    XMapWindow(display, childWindow);
    XFlush(display);
    
    qInfo() << "X11EmbedHelper: Window embedded successfully";
    return true;
#else
    Q_UNUSED(child)
    Q_UNUSED(parent)
    return false;
#endif
}

bool X11EmbedHelper::resizeWindow(unsigned long window, int width, int height)
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

bool X11EmbedHelper::sendKey(unsigned long window, const QString &key)
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
