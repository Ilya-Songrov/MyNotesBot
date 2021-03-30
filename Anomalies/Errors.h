#pragma once

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>

#include "ExceptionHandler.h"

class Errors : public QObject
{
    Q_OBJECT

public:
    static Errors& getInstance();

    void sendException(const ExceptionHandler &exceptionHandler);
    void sendException(std::exception &exception);
    void sendException(const QByteArray &text);

signals:
    void signalExceptionOccurred();

private:
    Errors(QObject *parent = nullptr);
};

