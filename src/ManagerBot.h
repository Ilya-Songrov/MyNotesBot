#pragma once

#include <QObject>
#include <QString>
#include <QVariant>

#include "logsaver.h"
#include "Adjutants/AppTranslator.h"
#include "FileWorker.h"
#include "GlobalData/GlobalData.h"
#include "Content/content.h"
#include "Content/chatactions.h"
#include "BotPlaces/PlaceStart/placestart.h"
#include "BotPlaces/PlaceNotes/PlaceNotes.h"
#include "BotPlaces/PlaceAdditional/PlaceAdditional.h"

#include <tgbot/tgbot.h>
using namespace TgBot;

class ManagerBot : public QObject
{
    Q_OBJECT

public:
    explicit ManagerBot(const QString token = QString(), QObject *parent = nullptr);

    void startBot();

private:
    void setSettings();
    void anyMessageWasWrited(const Message::Ptr message);
    void callbackQueryWasWrited(const CallbackQuery::Ptr callbackQuery);

    ChatActions getChatActions(const std::int64_t chat_id, const std::string &currentText);
    void changeObjPtrPlaceBot(const Content::Place place);
    QString getTokenFromFile();

    void printChatActions(const QString &header, const ChatActions &chatActions, const std::string &messageText);

private:
    LogSaver logSaver;
    AppTranslator appTranslator; // TODO: rewrite this
    PlaceStart          *placeStart     ;
    PlaceNotes          *placeNotes     ;
    PlaceAdditional     *placeAdditional;
    PlaceAbstract       *placeBot       ;
};

