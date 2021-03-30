#pragma once

#include <QObject>
#include <QCoreApplication>
#include <QFile>
#include <QDebug>
#include <QException>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>

class ExceptionHandler : public QException
{

public:
    explicit ExceptionHandler(const char* msg);
    explicit ExceptionHandler(const QString &msg = QString());
    virtual ~ExceptionHandler() throw();

    ExceptionHandler(const ExceptionHandler& other);
    ExceptionHandler& operator=(const ExceptionHandler& other);


    void raise() const override;
    ExceptionHandler* clone() const override;

    QString message() const;

    void writeException() const;
    static void writeException(std::exception &exception);
    static void writeException(const QByteArray &text);

private:
    static void createFileException(const QByteArray &arr);

private:
    QByteArray msg;
};
