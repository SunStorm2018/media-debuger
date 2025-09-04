#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <common/common.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "MediaDebuger_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // Get the singleton instance
    Common* common = Common::instance();

    // Initialize if not already initialized
    if (!common->isInitialized()) {
        common->initialize();
    }

    MainWindow w;
    w.show();
    return a.exec();
}
