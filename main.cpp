#include <QCoreApplication>

#include "ApplicationNotify.h"
#include "ManagerBot.h"

#define VERSION_MAJOR    "0"
#define VERSION_MINOR    "4"
#define VERSION_BUILD    "1"
#define APP_VERSION QString(VERSION_MAJOR) + "." + QString(VERSION_MINOR) + "." + QString(VERSION_BUILD)

int main(int argc, char *argv[])
{
    try {
        ApplicationNotify<QCoreApplication> app(argc, argv);
        app.setApplicationVersion(APP_VERSION);
//        QObject::connect(&Errors::getInstance(), &Errors::signalExceptionOccurred, &app, &decltype(app)::quit);

        QCommandLineParser parser;
        parser.setApplicationDescription("App description");
        parser.addHelpOption();
        parser.addVersionOption();
        parser.process(app);

        ManagerBot managerBot;
        managerBot.startBot();
        return app.exec();
    }
    catch (ExceptionHandler &exceptionHandler) {
        std::cerr << qPrintable(exceptionHandler.message()) << std::endl;
    }
    catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught (...)" << std::endl;
    }
    return EXIT_FAILURE;
}
