QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/common.cpp \
    common/flowlayout.cpp \
    common/tableheadermanager.cpp \
    common/zffprobe.cpp \
    common/zlogger.cpp \
    common/zwindowhelper.cpp \
    model/fileshistorymodel.cpp \
    model/logmodel.cpp \
    model/mediainfotabelmodel.cpp \
    model/multicolumnsearchproxymodel.cpp \
    widgets/baseformatwg.cpp \
    widgets/fileswg.cpp \
    widgets/folderswg.cpp \
    widgets/globalconfingwg.cpp \
    widgets/infowidgets.cpp \
    main.cpp \
    mainwindow.cpp \
    common/zprocess.cpp \
    widgets/jsonformatwg.cpp \
    widgets/logwg.cpp \
    widgets/playerwg.cpp \
    widgets/progressdialog.cpp \
    widgets/searchwg.cpp \
    widgets/tabelformatwg.cpp \
    zcodecflagparser.cpp

HEADERS += \
    common/common.h \
    common/flowlayout.h \
    common/tableheadermanager.h \
    common/zffprobe.h \
    common/zlogger.h \
    common/zwindowhelper.h \
    model/fileshistorymodel.h \
    model/logmodel.h \
    model/mediainfotabelmodel.h \
    model/multicolumnsearchproxymodel.h \
    widgets/baseformatwg.h \
    widgets/fileswg.h \
    widgets/folderswg.h \
    widgets/globalconfingwg.h \
    widgets/infowidgets.h \
    mainwindow.h \
    common/zprocess.h \
    widgets/jsonformatwg.h \
    widgets/logwg.h \
    widgets/playerwg.h \
    widgets/progressdialog.h \
    widgets/searchwg.h \
    widgets/tabelformatwg.h \
    zcodecflagparser.h

FORMS += \
    widgets/fileswg.ui \
    widgets/folderswg.ui \
    widgets/globalconfingwg.ui \
    widgets/infowidgets.ui \
    mainwindow.ui \
    widgets/jsonformatwg.ui \
    widgets/logwg.ui \
    widgets/playerwg.ui \
    widgets/searchwg.ui \
    widgets/tabelformatwg.ui

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
