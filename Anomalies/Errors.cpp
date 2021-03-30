#include "Errors.h"

Errors::Errors(QObject *parent) : QObject(parent)
{

}

Errors &Errors::getInstance()
{
    static Errors errors;
    return errors;
}

void Errors::sendException(const ExceptionHandler &exceptionHandler)
{
    exceptionHandler.writeException();
    emit signalExceptionOccurred();
}

void Errors::sendException(std::exception &exception)
{
    ExceptionHandler::writeException(exception);
    emit signalExceptionOccurred();
}

void Errors::sendException(const QByteArray &text)
{
    ExceptionHandler::writeException(text);
    emit signalExceptionOccurred();
}
