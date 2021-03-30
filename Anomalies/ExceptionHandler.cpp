#include "ExceptionHandler.h"


ExceptionHandler::ExceptionHandler(const char *msg) : QException()
  , msg(msg)
{

}

ExceptionHandler::ExceptionHandler(const QString &msg) : QException()
  , msg(msg.toUtf8())
{

}

ExceptionHandler::~ExceptionHandler() throw()
{

}

ExceptionHandler::ExceptionHandler(const ExceptionHandler &other) : QException()
{
    *this = other;
}

ExceptionHandler &ExceptionHandler::operator=(const ExceptionHandler &other)
{
    this->msg = other.msg;
    return *this;
}

void ExceptionHandler::raise() const
{
    throw *this;
}

ExceptionHandler *ExceptionHandler::clone() const
{
    return new ExceptionHandler(*this);
}

QString ExceptionHandler::message() const
{
    return msg;
}

void ExceptionHandler::writeException() const
{
    createFileException(msg);
}

void ExceptionHandler::writeException(std::exception &exception)
{
    createFileException(exception.what());
}

void ExceptionHandler::writeException(const QByteArray &text)
{
    createFileException(text);
}

void ExceptionHandler::createFileException(const QByteArray &arr)
{
    const QString pathFileException = QString("Exception_%1.json").arg(QCoreApplication::applicationName());
    qCritical() << QString("Write exception: (%1), pathFileException: (%2)").arg(arr.data(), pathFileException) << Qt::endl;
    QFile file(pathFileException);
    if (!file.open(QFile::WriteOnly | QFile::Append)){
        qCritical() << "Error QFile::WriteOnly | QFile::Append" << file.fileName() << file.errorString() << Qt::endl;
        return;
    }
    QJsonObject obj;
    obj.insert("exception", QString(arr));
    obj.insert("date_time", QDateTime::currentDateTime().toString("yyyy:MM:dd - hh:mm:ss:zzz"));
    file.write(QJsonDocument(obj).toJson());
    file.close();
}
