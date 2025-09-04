QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/common.cpp \
    common/zffprobe.cpp \
    common/zwindowhelper.cpp \
    widgets/globalconfingwg.cpp \
    widgets/infowidgets.cpp \
    main.cpp \
    mainwindow.cpp \
    common/zprocess.cpp \
    widgets/jsonformatwg.cpp \
    zcodecflagparser.cpp

HEADERS += \
    common/common.h \
    common/zffprobe.h \
    common/zwindowhelper.h \
    widgets/globalconfingwg.h \
    widgets/infowidgets.h \
    mainwindow.h \
    common/zprocess.h \
    widgets/jsonformatwg.h \
    zcodecflagparser.h

FORMS += \
    widgets/globalconfingwg.ui \
    widgets/infowidgets.ui \
    mainwindow.ui \
    widgets/jsonformatwg.ui

TRANSLATIONS += \
    MediaDebuger_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

include(third_part/QJsonModel/QJsonModel.pri)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources/resources.qrc
