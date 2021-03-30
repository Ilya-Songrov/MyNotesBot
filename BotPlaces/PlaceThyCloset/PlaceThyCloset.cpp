#include "PlaceThyCloset.h"

PlaceThyCloset::PlaceThyCloset(QObject *parent) : PlaceAbstract(parent)
{

}

void PlaceThyCloset::slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::ThyCloset_AddPrayerNeed:
        onAddPrayerNeed(message);
        break;
    case Content::ThyCloset_AddAnswerOfGod:
        onAddAnswerOfGod(message);
        break;
    case Content::ThyCloset_ListPrayerNeed:
        onListPrayerNeed(message);
        break;
    case Content::MultiPlace_AnyMessage:
        onAnyMessage(message);
        break;
    default:
        PlaceAbstract::slotOnCommand(message, chatInfo);
    }
}

void PlaceThyCloset::slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::MultiPlace_AnyCallbackQuery:
        onAnyCallbackQuery(callbackQuery);
        break;
    default:
        PlaceAbstract::slotOnCallbackQuery(callbackQuery, chatInfo);
    }
}

void PlaceThyCloset::onAddPrayerNeed(const Message::Ptr &message)
{
    static const auto answer { QObject::tr("Write your prayer need:").toStdString() };
    bot->getApi().sendMessage(message->chat->id, answer);
}

void PlaceThyCloset::onAddAnswerOfGod(const Message::Ptr &message)
{
    static const auto answer { QObject::tr("Select the prayer need:").toStdString() };
    const auto vecNeeds = managerDatabase->getVecPrayerNeeds(message->chat->id);
    QList<QPair<QString, QString> > listButtons;
    for (const auto &prayerNeed: vecNeeds) {
        listButtons.append(qMakePair(prayerNeed.need.left(15) + " ...", prayerNeed.need_id));
    }
    const auto inlineButtonPrayerNeed = createOneColumnInlineKeyboardMarkup(listButtons);
    sendInlineKeyboardMarkupMessage(message->chat->id, answer, inlineButtonPrayerNeed);
}

void PlaceThyCloset::onListPrayerNeed(const Message::Ptr &message)
{
    bot->getApi().sendMessage(message->chat->id, getListPrayerNeeds(message), false, 0, getStartingButtons());
}

void PlaceThyCloset::onAnyMessage(const Message::Ptr &message)
{
    const auto chat_id = message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::ThyCloset_AddPrayerNeed)) {
        managerDatabase->addPrayerNeed(message->text, message->chat->id);
        sendStartingMessage(chat_id, getListPrayerNeeds(message));
    }
    else if (chatContainsLastCommand(chat_id, Content::ThyCloset_WriteAnswerOfGod)) {
        const auto lastNeedId = mapAllChats->value(chat_id).lastNeedId;
        if (lastNeedId != -1) {
            managerDatabase->addAnswerOfGod(message->text, lastNeedId);
        }
        sendStartingMessage(chat_id, getListPrayerNeeds(message));
    }
    else if (QString::fromStdString(message->text).toLower() == "ping") {
        sendStartingMessage(chat_id, "Pong!");
    }
}

void PlaceThyCloset::onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery)
{
    const auto chat_id = callbackQuery->message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::ThyCloset_AddAnswerOfGod)) {
        static const auto answer { QObject::tr("Write answer of God:").toStdString() };
        bool ok;
        const int need_id = QString::fromStdString(callbackQuery->data).toInt(&ok);
        if (ok) {
            auto chatInfo = mapAllChats->value(chat_id);
            chatInfo.currentCommand = Content::Command::ThyCloset_WriteAnswerOfGod;
            chatInfo.lastNeedId = need_id;
            mapAllChats->insert(chat_id, chatInfo);
        }
        bot->getApi().answerCallbackQuery(callbackQuery->id);
        bot->getApi().sendMessage(chat_id, answer);
    }
}

std::string PlaceThyCloset::getListPrayerNeeds(const Message::Ptr &message)
{
    const QString answer { QObject::tr("List prayers:") + "\n" + managerDatabase->getListPrayerNeeds(message->chat->id).join('\n') };
    return answer.toStdString();
}

