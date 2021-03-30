#include "PlaceMyNotes.h"

PlaceMyNotes::PlaceMyNotes(QObject *parent) : PlaceAbstract(parent)
{

}

void PlaceMyNotes::slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::MyNotes_AddNote:
        onAddNote(message);
        break;
    case Content::MyNotes_RemoveNote:
        onRemoveNote(message);
        break;
    case Content::MultiPlace_AnyMessage:
        onAnyMessage(message);
        break;
    default:
        PlaceAbstract::slotOnCommand(message, chatInfo);
    }
}

void PlaceMyNotes::slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::MultiPlace_AnyCallbackQuery:
        onAnyCallbackQuery(callbackQuery);
        break;
    default:
        PlaceAbstract::slotOnCallbackQuery(callbackQuery, chatInfo);
    }
}

void PlaceMyNotes::onAddNote(const Message::Ptr &message)
{
    static const auto answer { QObject::tr("Write your prayer need:").toStdString() };
    bot->getApi().sendMessage(message->chat->id, answer);
}

void PlaceMyNotes::onRemoveNote(const Message::Ptr &message)
{
    static const auto answer { QObject::tr("Select the prayer need:").toStdString() };
    const auto vecNeeds = managerDatabase->getVecMyNotes(message->chat->id);
    QList<QPair<QString, QString> > listButtons;
    for (const auto &prayerNeed: vecNeeds) {
        listButtons.append(qMakePair(prayerNeed.need.left(15) + " ...", prayerNeed.need_id));
    }
    const auto inlineButtonPrayerNeed = createOneColumnInlineKeyboardMarkup(listButtons);
    sendInlineKeyboardMarkupMessage(message->chat->id, answer, inlineButtonPrayerNeed);
}

void PlaceMyNotes::onListPrayerNeed(const Message::Ptr &message)
{
    bot->getApi().sendMessage(message->chat->id, getListPrayerNeeds(message), false, 0, getStartingButtons(message->chat->id));
}

void PlaceMyNotes::onAnyMessage(const Message::Ptr &message)
{
    const auto chat_id = message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::MyNotes_AddNote)) {
        managerDatabase->addNote(message->text, message->chat->id);
        sendStartingMessage(chat_id, getListPrayerNeeds(message));
    }
//    else if (chatContainsLastCommand(chat_id, Content::ThyCloset_WriteAnswerOfGod)) {
//        const auto lastNeedId = mapAllChats->value(chat_id).lastNeedId;
//        if (lastNeedId != -1) {
//            managerDatabase->addAnswerOfGod(message->text, lastNeedId);
//        }
//        sendStartingMessage(chat_id, getListPrayerNeeds(message));
//    }
    else if (QString::fromStdString(message->text).toLower() == "ping") {
        sendStartingMessage(chat_id, "Pong!");
    }
}

void PlaceMyNotes::onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery)
{
    const auto chat_id = callbackQuery->message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::MyNotes_GroupNotes)) {
        static const auto answer { QObject::tr("Write answer of God:").toStdString() };
        bool ok;
        const int need_id = QString::fromStdString(callbackQuery->data).toInt(&ok);
        if (ok) {
            auto chatInfo = mapAllChats->value(chat_id);
            chatInfo.currentCommand = Content::Command::MyNotes_GroupNotes;
            chatInfo.lastNeedId = need_id;
            mapAllChats->insert(chat_id, chatInfo);
        }
        bot->getApi().answerCallbackQuery(callbackQuery->id);
        bot->getApi().sendMessage(chat_id, answer);
    }
}

std::string PlaceMyNotes::getListPrayerNeeds(const Message::Ptr &message)
{
    const QString answer { QObject::tr("List prayers:") + "\n" + managerDatabase->getListMyNotes(message->chat->id).join('\n') };
    return answer.toStdString();
}

