# SPDX-FileCopyrightText: 2025 zhang hongyuan <2063218120@qq.com>
# SPDX-License-Identifier: MIT

QT += core gui concurrent

TARGET = media-debuger-qt5

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Add include paths for proper header resolution
INCLUDEPATH += src

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/common/zcommandexecutor.cpp \
    src/common/common.cpp \
    src/common/zflowlayout.cpp \
    src/common/zmultiselectmenu.cpp \
    src/common/ztableheadermanager.cpp \
    src/common/zffprobe.cpp \
    src/common/zffmpeg.cpp \
    src/common/zffplay.cpp \
    src/common/zlogger.cpp \
    src/common/ztexteditor.cpp \
    src/common/ztexthighlighter.cpp \
    src/common/zwindowhelper.cpp \
    src/model/fileshistorymodel.cpp \
    src/model/logmodel.cpp \
    src/model/mediainfotabelmodel.cpp \
    src/model/multicolumnsearchproxymodel.cpp \
    src/widgets/basefmtwg.cpp \
    src/widgets/exportwg.cpp \
    src/widgets/ffmpegbuildtool.cpp \
    src/widgets/fileswg.cpp \
    src/widgets/folderswg.cpp \
    src/widgets/globalconfingwg.cpp \
    src/widgets/helpquerywg.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/widgets/infotablewg.cpp \
    src/widgets/jsonfmtwg.cpp \
    src/widgets/logwg.cpp \
    src/widgets/playerwg.cpp \
    src/widgets/progressdlg.cpp \
    src/widgets/searchwg.cpp \
    src/widgets/tabconfigwg.cpp \
    src/widgets/tabelfmtwg.cpp \
    src/widgets/x11embedhelper.cpp

HEADERS += \
    src/common/zcommandexecutor.h \
    src/common/common.h \
    src/common/zflowlayout.h \
    src/common/qtcompat.h \
    src/common/zmultiselectmenu.h \
    src/common/ztableheadermanager.h \
    src/common/zffprobe.h \
    src/common/zffmpeg.h \
    src/common/zffplay.h \
    src/common/zlogger.h \
    src/common/ztexteditor.h \
    src/common/ztexthighlighter.h \
    src/common/zwindowhelper.h \
    src/model/fileshistorymodel.h \
    src/model/logmodel.h \
    src/model/mediainfotabelmodel.h \
    src/model/multicolumnsearchproxymodel.h \
    src/widgets/basefmtwg.h \
    src/widgets/exportwg.h \
    src/widgets/ffmpegbuildtool.h \
    src/widgets/fileswg.h \
    src/widgets/folderswg.h \
    src/widgets/globalconfingwg.h \
    src/widgets/helpquerywg.h \
    src/mainwindow.h \
    src/widgets/infotablewg.h \
    src/widgets/jsonfmtwg.h \
    src/widgets/logwg.h \
    src/widgets/playerwg.h \
    src/widgets/progressdlg.h \
    src/widgets/searchwg.h \
    src/widgets/tabconfigwg.h \
    src/widgets/tabelfmtwg.h \
    src/widgets/x11embedhelper.h

FORMS += \
    src/widgets/exportwg.ui \
    src/widgets/ffmpegbuildtool.ui \
    src/widgets/fileswg.ui \
    src/widgets/folderswg.ui \
    src/widgets/globalconfingwg.ui \
    src/widgets/helpquerywg.ui \
    src/mainwindow.ui \
    src/widgets/infotablewg.ui \
    src/widgets/jsonfmtwg.ui \
    src/widgets/logwg.ui \
    src/widgets/playerwg.ui \
    src/widgets/searchwg.ui \
    src/widgets/tabconfigwg.ui \
    src/widgets/tabelfmtwg.ui

TRANSLATIONS += \
    translations/media-debuger_en_US.ts \
    translations/media-debuger_zh_CN.ts

CONFIG += lrelease
CONFIG += embed_translations

include(third_part/QJsonModel/QJsonModel.pri)

CONFIG += link_pkgconfig
PKGCONFIG += \
    libavcodec \
    libavformat \
    libavutil \
    libswscale \
    libswresample \
    libavfilter

# Add X11 support for Linux
unix:!macx {
    LIBS += -lX11
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets/resources.qrc

isEmpty(PREFIX) {
    PREFIX = /usr
}

isEmpty(BINDIR): BINDIR = $$PREFIX/bin
isEmpty(ICONDIR): ICONDIR = $$PREFIX/share/icons/hicolor/scalable/apps
isEmpty(APPDIR): APPDIR = $$PREFIX/share/applications
isEmpty(DOCDIR): DOCDIR = $$PREFIX/share/doc/media-debuger
# isEmpty(CONTEXTMENUDIR): CONTEXTMENUDIR = $$PREFIX/share/applications/context-menus

target.path = $$BINDIR
icon.path = $$ICONDIR
desktop.path = $$APPDIR
doc.path = $$DOCDIR
# contextmenu.path = $$CONTEXTMENUDIR

icon.files = assets/128x128/media-debuger-logo.svg
desktop.files = assets/media-debuger.desktop
doc.files = README.md LICENSE
# contextmenu.files = assets/media-debuger.conf

INSTALLS += target icon desktop doc
