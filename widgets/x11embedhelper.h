#ifndef X11EMBEDHELPER_H
#define X11EMBEDHELPER_H

#include <QObject>
#include <QString>

#ifdef Q_OS_LINUX
// Forward declaration for X11 types
typedef struct _XDisplay Display;
typedef unsigned long Window;
#endif

enum MouseButton {
    LeftButton,
    MiddleButton,
    RightButton
};

class X11EmbedHelper : public QObject
{
    Q_OBJECT

public:
    explicit X11EmbedHelper(QObject *parent = nullptr);
    ~X11EmbedHelper();

    bool initialize();
    void cleanup();
    unsigned long findWindow(const QString &title);
    bool embedWindow(unsigned long child, unsigned long parent);
    bool resizeWindow(unsigned long window, int width, int height);
    bool sendKey(unsigned long window, const QString &key);
    bool showWindow(unsigned long window);
    bool sendMouseClick(unsigned long window, int x, int y, MouseButton button);

private:
#ifdef Q_OS_LINUX
    unsigned long findWindowRecursive(Display *display, Window window, const QString &title);
#endif
    void *m_display;
};

#endif // X11EMBEDHELPER_H
