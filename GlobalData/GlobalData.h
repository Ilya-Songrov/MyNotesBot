#pragma once

#include <QObject>
#include <QCoreApplication>

#include <managerdatabase.h>
#include <tgbot/tgbot.h>
using namespace TgBot;

extern std::unique_ptr<Bot> bot;
extern std::unique_ptr<ManagerDatabase> managerDatabase;

void initGlobalData(const QString &token);

