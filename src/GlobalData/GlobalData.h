#pragma once

#include <QObject>
#include <QCoreApplication>

#include <managerdb.h>
#include <tgbot/tgbot.h>
using namespace TgBot;

extern std::unique_ptr<Bot> bot;
extern std::unique_ptr<ManagerDb> managerDb;

void initGlobalData(const QString &token);

