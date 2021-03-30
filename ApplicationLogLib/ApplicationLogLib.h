#pragma once

#include <QDebug>
#include <QCoreApplication>
#include <QDir>

#include "ApplicationLogLib_global.h"

/*
    ### add to .pro file ###
        HEADERS += \
            ApplicationLogLib/ApplicationLogLib.h \
            ApplicationLogLib/ApplicationLogLib_global.h \

        unix:!macx: LIBS += -L$$PWD/ApplicationLogLib/ -lApplicationLogLib
        INCLUDEPATH += $$PWD/ApplicationLogLib
        DEPENDPATH  += $$PWD/ApplicationLogLib
        unix:!macx: PRE_TARGETDEPS += $$PWD/ApplicationLogLib/libApplicationLogLib.so
        #QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"   # OR
        QMAKE_RPATHDIR += ":'\$$ORIGIN'"            # OR

    ### for example in main.cpp ###
        #include "ApplicationLogLib/ApplicationLogLib.h"
        ApplicationLogLib applicationLogLib;
*/

#define DEFAULT_LOG_PATH QDir::currentPath() + QString("/Log/%1.log").arg(QCoreApplication::applicationName())

class Logbook;

class APPLICATIONLOGLIB_EXPORT ApplicationLogLib : QObject
{
    Q_OBJECT
    Q_CLASSINFO("Version", "2.0.2")

    Logbook *logbook;
public:
    explicit ApplicationLogLib(const QString &pathFileLog = DEFAULT_LOG_PATH, QObject *parent = nullptr);

    static QString getVersionClass();
};
