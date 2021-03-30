#include "ManagerBot.h"


ManagerBot::ManagerBot(const QString token, QObject *parent) : QObject(parent)
  , appTranslator(":/translationFiles/appTranslator_ru_UA.qm")
  , mapAllChats(new QMap<std::uint64_t, ChatInfo>())
{
    Content::initContent();
    initGlobalData(token.isEmpty() ? getTokenFromFile() : token);
    PlaceAbstract::initMapAllChats(mapAllChats);

    placeThyCloset      = new PlaceThyCloset    (this);
    placeAdditional     = new PlaceAdditional   (this);
    placeBot            = placeThyCloset;
    setSettings();
}

void ManagerBot::startBot()
{
    //    Content::initContent();

//    signal(SIGINT, [](int s) {
//        printf("SIGINT got\n");
//        exit(0);
//    });

    qDebug("Token: %s\n", bot->getToken().c_str());
    try {
        qDebug("Bot username: %s\n", bot->getApi().getMe()->username.c_str());
        bot->getApi().deleteWebhook();

        TgLongPoll longPoll(*bot.get());
        while (true) {
            qDebug("Long poll started\n");
            longPoll.start();
        }
    } catch (std::exception& e) {
        qDebug("error: %s\n", e.what());
    }
}

void ManagerBot::setSettings()
{
    bot->getEvents().onAnyMessage(std::bind(&ManagerBot::anyMessageWasWrite, this, std::placeholders::_1));
    bot->getEvents().onCallbackQuery(std::bind(&ManagerBot::callbackQueryWasWrite, this, std::placeholders::_1));

    bot->getEvents().onInlineQuery([](const InlineQuery::Ptr){ qDebug() << "onInlineQuery" << Qt::endl; });
    bot->getEvents().onChosenInlineResult([](const ChosenInlineResult::Ptr){ qDebug() << "onChosenInlineResult" << Qt::endl; });
//    bot->getEvents().onCallbackQuery([](const CallbackQuery::Ptr &callbackQuery){ qDebug() << "onCallbackQuery" << callbackQuery->data.c_str() << Qt::endl; });
}

void ManagerBot::anyMessageWasWrite(const Message::Ptr message)
{
    const auto chatInfo = getChatInfo(message->chat->id, message->text);
    printChatInfo(QString(__FUNCTION__), chatInfo, message->text);
    mapAllChats->insert(message->chat->id, chatInfo);
    changeObjPtrPlaceBot(chatInfo.currentPlace);
    placeBot->slotOnCommand(message, chatInfo);
}

void ManagerBot::callbackQueryWasWrite(const CallbackQuery::Ptr callbackQuery)
{
    const auto chatInfo = getChatInfo(callbackQuery->message->chat->id, callbackQuery->data);
    printChatInfo(QString(__FUNCTION__), chatInfo, callbackQuery->message->text);
    mapAllChats->insert(callbackQuery->message->chat->id, chatInfo);
    changeObjPtrPlaceBot(chatInfo.currentPlace);
    placeBot->slotOnCallbackQuery(callbackQuery, chatInfo);
}

ChatInfo ManagerBot::getChatInfo(const int64_t chat_id, const std::string &currentText)
{
    const Content::PlaceCommand currentPlaceCommand = Content::getPlaceCommand(currentText);
    ChatInfo chatInfo = mapAllChats->value(chat_id);
    chatInfo.lastPlace      = chatInfo.currentPlace;
    chatInfo.lastCommand    = chatInfo.currentCommand;
    chatInfo.currentPlace   = currentPlaceCommand.place;
    chatInfo.currentCommand = currentPlaceCommand.command;
    return chatInfo;
}

void ManagerBot::changeObjPtrPlaceBot(const Content::Place place)
{
    switch (place) {
    case Content::Place::ThyCloset:
        placeBot = placeThyCloset;
        break;
    case Content::Place::Additional:
        placeBot = placeAdditional;
        break;
    case Content::Place::MultiPlace:
        break;
    default:
        break;
    }
}

QString ManagerBot::getTokenFromFile()
{
    const QJsonDocument doc = FileWorker::readFileJson("../config.json");
    const QJsonObject obj = doc.object();
    return obj.value("token").toString();
}

void ManagerBot::printChatInfo(const QString &header, const ChatInfo &chatInfo, const std::string &messageText)
{
    static const QChar placeholder { '-' };
    static const int lenghtSymbols = 48;
    static const int justified = lenghtSymbols / 2 + header.size() / 2;
    const QString frameHeader = QString(header).leftJustified(justified, '-').rightJustified(lenghtSymbols, placeholder);

    qDebug() << frameHeader << Qt::endl;
    qDebug() << "lastPlace      :" << chatInfo.lastPlace        ;
    qDebug() << "lastCommand    :" << chatInfo.lastCommand      ;
    qDebug() << "currentPlace   :" << chatInfo.currentPlace     ;
    qDebug() << "currentCommand :" << chatInfo.currentCommand   ;
    qDebug() << "lastNeedId     :" << chatInfo.lastNeedId       ;
    qDebug() << "messageText    :" << messageText.c_str()       ;
    qDebug() << Qt::endl << frameHeader << Qt::endl;
}
