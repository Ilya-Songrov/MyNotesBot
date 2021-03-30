#pragma once

#include <QObject>

#include "ExceptionHandler.h"
#include "Errors.h"

template<class T>
class ApplicationNotify : public T
{
public:
    ApplicationNotify(int &argc, char **argv, int flag = T::ApplicationFlags) : T(argc, argv, flag){}
    virtual ~ApplicationNotify(){}

    bool notify(QObject *receiver, QEvent *event) override {
        try {
            return T::notify(receiver, event);
        }
        catch (ExceptionHandler &exceptionHandler) {
            exceptionHandler.writeException();
            Errors::getInstance().signalExceptionOccurred();
        }
        catch (std::exception &exception) {
            ExceptionHandler::writeException(exception);
            Errors::getInstance().signalExceptionOccurred();
        }
        catch (...) {
            ExceptionHandler::writeException("Unknown exception caught (...)");
            Errors::getInstance().signalExceptionOccurred();
        }
        return false;
    }
};

