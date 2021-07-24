/**************************************************************************
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/.
**
**************************************************************************/

#include <QCoreApplication>

#include "applicationnotify.h"
#include "ManagerBot.h"

#define VERSION_MAJOR    "1"
#define VERSION_MINOR    "0"
#define VERSION_BUILD    "0"
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
    catch (ExceptionError &exceptionError) {
        std::cerr << qPrintable(exceptionError.message()) << std::endl;
    }
    catch (std::exception &exception) {
        std::cerr << exception.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception caught (...)" << std::endl;
    }
    return EXIT_FAILURE;
}
