#include "GlobalData.h"

std::unique_ptr<Bot> bot(nullptr);
std::unique_ptr<ManagerDb> managerDb(nullptr);

void initGlobalData(const QString &token)
{
    bot.reset(new Bot(token.toStdString()));
    managerDb.reset(new ManagerDb(QString("%1Database.db").arg(QCoreApplication::applicationName())));
}
