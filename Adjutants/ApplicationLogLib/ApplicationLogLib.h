#pragma once

#include <QDebug>
#include <QCoreApplication>
#include <QDir>

#include "ApplicationLogLib_global.h"

#define DEFAULT_LOG_PATH QDir::currentPath() + QString("/Log/%1.log").arg(QCoreApplication::applicationName())

class Logbook;

class APPLICATIONLOGLIB_EXPORT ApplicationLogLib : QObject
{
    Q_OBJECT
    Q_CLASSINFO("Version", "2.0.0")

    Logbook *logbook;
public:
    explicit ApplicationLogLib(const QString &pathFileLog = DEFAULT_LOG_PATH, QObject *parent = nullptr);

    static QString getVersionClass();
};
