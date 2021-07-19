#include "GlobalData.h"

std::unique_ptr<Bot> bot(nullptr);
std::unique_ptr<ManagerDatabase> managerDatabase(nullptr);

void initGlobalData(const QString &token)
{
    bot.reset(new Bot(token.toStdString()));
    managerDatabase.reset(new ManagerDatabase(QString("%1Database.db").arg(QCoreApplication::applicationName())));
}
