QT += core gui concurrent

TARGET = media-debuger

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/checkboxsynchronizer.cpp \
    common/commandexecutor.cpp \
    common/common.cpp \
    common/flowlayout.cpp \
    common/functioninterface.cpp \
    common/lineeditsynchronizer.cpp \
    common/spinboxsynchronizer.cpp \
    common/statesynchronizer.cpp \
    common/tableheadermanager.cpp \
    common/zffprobe.cpp \
    common/zlogger.cpp \
    common/ztexteditor.cpp \
    common/ztexthighlighter.cpp \
    common/zwindowhelper.cpp \
    model/fileshistorymodel.cpp \
    model/logmodel.cpp \
    model/mediainfotabelmodel.cpp \
    model/multicolumnsearchproxymodel.cpp \
    widgets/baseformatwg.cpp \
    widgets/exportwg.cpp \
    widgets/ffmpegbuildtool.cpp \
    widgets/fileswg.cpp \
    widgets/folderswg.cpp \
    widgets/globalconfingwg.cpp \
    widgets/helpquerywg.cpp \
    widgets/infowidgets.cpp \
    main.cpp \
    mainwindow.cpp \
    widgets/jsonformatwg.cpp \
    widgets/logwg.cpp \
    widgets/playerwg.cpp \
    widgets/progressdlg.cpp \
    widgets/searchwg.cpp \
    widgets/tabconfigwg.cpp \
    widgets/tabelformatwg.cpp \
    widgets/x11embedhelper.cpp

HEADERS += \
    common/checkboxsynchronizer.h \
    common/commandexecutor.h \
    common/common.h \
    common/flowlayout.h \
    common/functioninterface.h \
    common/lineeditsynchronizer.h \
    common/spinboxsynchronizer.h \
    common/statesynchronizer.h \
    common/synchronizermacros.h \
    common/tableheadermanager.h \
    common/zffprobe.h \
    common/zlogger.h \
    common/ztexteditor.h \
    common/ztexthighlighter.h \
    common/zwindowhelper.h \
    model/fileshistorymodel.h \
    model/logmodel.h \
    model/mediainfotabelmodel.h \
    model/multicolumnsearchproxymodel.h \
    widgets/baseformatwg.h \
    widgets/exportwg.h \
    widgets/ffmpegbuildtool.h \
    widgets/fileswg.h \
    widgets/folderswg.h \
    widgets/globalconfingwg.h \
    widgets/helpquerywg.h \
    widgets/infowidgets.h \
    mainwindow.h \
    widgets/jsonformatwg.h \
    widgets/logwg.h \
    widgets/playerwg.h \
    widgets/progressdlg.h \
    widgets/searchwg.h \
    widgets/tabconfigwg.h \
    widgets/tabelformatwg.h \
    widgets/x11embedhelper.h

FORMS += \
    widgets/exportwg.ui \
    widgets/ffmpegbuildtool.ui \
    widgets/fileswg.ui \
    widgets/folderswg.ui \
    widgets/globalconfingwg.ui \
    widgets/helpquerywg.ui \
    widgets/infowidgets.ui \
    mainwindow.ui \
    widgets/jsonformatwg.ui \
    widgets/logwg.ui \
    widgets/playerwg.ui \
    widgets/searchwg.ui \
    widgets/tabconfigwg.ui \
    widgets/tabelformatwg.ui

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
