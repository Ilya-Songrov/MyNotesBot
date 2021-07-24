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

#include "ManagerBot.h"

ManagerBot::ManagerBot(const QString token, QObject *parent) : QObject(parent)
  , appTranslator(":/translationFiles/NotesBot_UA.qm")
{
    Content::initContent();
    initGlobalData(token.isEmpty() ? getTokenFromFile() : token);

    placeStart          = new PlaceStart    (this);
    placeNotes          = new PlaceNotes    (this);
    placeAdditional     = new PlaceAdditional   (this);
    placeBot            = placeNotes;
    setSettings();
}

void ManagerBot::startBot()
{
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
    bot->getEvents().onAnyMessage(std::bind(&ManagerBot::anyMessageWasWrited, this, std::placeholders::_1));
    bot->getEvents().onCallbackQuery(std::bind(&ManagerBot::callbackQueryWasWrited, this, std::placeholders::_1));

    bot->getEvents().onInlineQuery([](const InlineQuery::Ptr){ qDebug() << "onInlineQuery" << Qt::endl; });
    bot->getEvents().onChosenInlineResult([](const ChosenInlineResult::Ptr){ qDebug() << "onChosenInlineResult" << Qt::endl; });
//    bot->getEvents().onCallbackQuery([](const CallbackQuery::Ptr &callbackQuery){ qDebug() << "onCallbackQuery" << callbackQuery->data.c_str() << Qt::endl; });
}

void ManagerBot::anyMessageWasWrited(const Message::Ptr message)
{
    const auto chatActions = getChatActions(message->chat->id, message->text);
    printChatActions(QString(__FUNCTION__), chatActions, message->text);
    managerDb->setChatActions(chatActions, message->chat->id);
    changeObjPtrPlaceBot(chatActions.currentPlace);
    placeBot->slotOnCommand(message, chatActions);
}

void ManagerBot::callbackQueryWasWrited(const CallbackQuery::Ptr callbackQuery)
{
    const auto chatActions = getChatActions(callbackQuery->message->chat->id, callbackQuery->data);
    printChatActions(QString(__FUNCTION__), chatActions, callbackQuery->message->text);
    managerDb->setChatActions(chatActions, callbackQuery->message->chat->id);
    changeObjPtrPlaceBot(chatActions.currentPlace);
    placeBot->slotOnCallbackQuery(callbackQuery, chatActions);
}

ChatActions ManagerBot::getChatActions(const int64_t chat_id, const std::string &currentText)
{
    const Content::PlaceCommand currentPlaceCommand = Content::getPlaceCommand(currentText);
    ChatActions chatActions     = managerDb->getChatActions(chat_id);
    chatActions.lastPlace       = chatActions.currentPlace;
    chatActions.lastCommand     = chatActions.currentCommand;
    chatActions.currentPlace    = currentPlaceCommand.place;
    chatActions.currentCommand  = currentPlaceCommand.command;
    return chatActions;
}

void ManagerBot::changeObjPtrPlaceBot(const Content::Place place)
{
    if (place == Content::Place::Start) {
        placeBot = placeStart;
    }
    else if (place == Content::Place::Notes) {
        placeBot = placeNotes;
    }
    else if (place == Content::Place::Additional) {
        placeBot = placeAdditional;
    }
    else {
        placeBot = placeNotes;
    }
}

QString ManagerBot::getTokenFromFile()
{
#ifdef QT_DEBUG
    const QJsonDocument doc = FileWorker::readFileJson("../../config.json");
#else
    const QJsonDocument doc = FileWorker::readFileJson("../config.json");
#endif
    const QJsonObject obj = doc.object();
    return obj.value("token").toString();
}

void ManagerBot::printChatActions(const QString &header, const ChatActions &chatActions, const std::string &messageText)
{
    static const QChar placeholder { '-' };
    static const int lenghtSymbols = 48;
    static const int justified = lenghtSymbols / 2 + header.size() / 2;
    const QString frameHeader = QString(header).leftJustified(justified, '-').rightJustified(lenghtSymbols, placeholder);

    qDebug() << frameHeader << Qt::endl;
    qDebug() << chatActions;
    qDebug() << "messageText:" << messageText.c_str()       ;
    qDebug() << Qt::endl << frameHeader << Qt::endl;
}
