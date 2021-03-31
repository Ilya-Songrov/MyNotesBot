#include "PlaceNotes.h"

PlaceNotes::PlaceNotes(QObject *parent) : PlaceAbstract(parent)
{

}

void PlaceNotes::slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::Notes_AddNote:
        onAddNote(message);
        break;
    case Content::Notes_RemoveNote:
        onRemoveNote(message);
        break;
    case Content::MultiPlace_AnyMessage:
        onAnyMessage(message);
        break;
    default:
        PlaceAbstract::slotOnCommand(message, chatInfo);
    }
}

void PlaceNotes::slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo)
{
    if (chatInfo.currentCommand == Content::Notes_NewGroup) {
        onAnyCallbackQuery(callbackQuery);
    }
    else if (chatInfo.currentCommand == Content::MultiPlace_AnyCallbackQuery) {
        onAnyCallbackQuery(callbackQuery);
    }
    else{
        PlaceAbstract::slotOnCallbackQuery(callbackQuery, chatInfo);
    }
}

void PlaceNotes::onAddNote(const Message::Ptr &message)
{
    static const auto answer { QObject::tr("Select group:").toStdString() };
    static const auto newGroup = Content::getCommandStr(Content::Notes_NewGroup);
    auto list = managerDatabase->getListGroups(message->chat->id);
    list.append(newGroup);
    const auto inlineButtonPrayerNeed = createOneColumnInlineKeyboardMarkup(list);
    sendInlineKeyboardMarkupMessage(message->chat->id, answer, inlineButtonPrayerNeed);
}

void PlaceNotes::onRemoveNote(const Message::Ptr &message)
{
//    static const auto answer { QObject::tr("Select the prayer need:").toStdString() };
//    const auto vec = managerDatabase->getVecNotes(message->chat->id);
//    QList<QPair<QString, QString> > listButtons;
//    for (const auto &n: vec) {
//        listButtons.append(qMakePair(n.need.left(15) + " ...", n.need_id));
//    }
//    const auto inlineButtonPrayerNeed = createOneColumnInlineKeyboardMarkup(listButtons);
//    sendInlineKeyboardMarkupMessage(message->chat->id, answer, inlineButtonPrayerNeed);
}

void PlaceNotes::onListPrayerNeed(const Message::Ptr &message)
{
    bot->getApi().sendMessage(message->chat->id, getListPrayerNeeds(message), false, 0, getMainMenuButtons(message->chat->id));
}

void PlaceNotes::onAnyMessage(const Message::Ptr &message)
{
    const auto chat_id = message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::Notes_NewGroup)) {
        managerDatabase->addNote(message->text, message->chat->id);
        sendMainMenuMessage(chat_id, getListPrayerNeeds(message));
    }
    else if (chatContainsLastCommand(chat_id, Content::ThyCloset_WriteAnswerOfGod)) {
        const auto lastNeedId = mapAllChats->value(chat_id).lastNeedId;
        if (lastNeedId != -1) {
            managerDatabase->addAnswerOfGod(message->text, lastNeedId);
        }
        sendStartingMessage(chat_id, getListPrayerNeeds(message));
    }
    else if (QString::fromStdString(message->text).toLower() == "ping") {
        sendMainMenuMessage(chat_id, "Pong!");
    }
}

void PlaceNotes::onNewGroupCallbackQuery(const CallbackQuery::Ptr &callbackQuery)
{
    static const auto answer { QObject::tr("Write name of group:").toStdString() };
    const auto chat_id = callbackQuery->message->chat->id;
    bot->getApi().answerCallbackQuery(callbackQuery->id);
    bot->getApi().sendMessage(chat_id, answer);
}

void PlaceNotes::onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery)
{
    const auto chat_id = callbackQuery->message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::Notes_AddNote)) {
        static const auto answer { QObject::tr("Write answer of God:").toStdString() };
        bool ok;
        const int need_id = QString::fromStdString(callbackQuery->data).toInt(&ok);
        if (ok) {
            auto chatInfo = mapAllChats->value(chat_id);
//            chatInfo.currentCommand = Content::Command::Notes_GroupNotes;
            chatInfo.lastNeedId = need_id;
            mapAllChats->insert(chat_id, chatInfo);
        }
        bot->getApi().answerCallbackQuery(callbackQuery->id);
        bot->getApi().sendMessage(chat_id, answer);
    }
}

std::string PlaceNotes::getListPrayerNeeds(const Message::Ptr &message)
{
//    const QString answer { QObject::tr("List prayers:") + "\n" + managerDatabase->getListNotes(message->chat->id).join('\n') };
//    return answer.toStdString();
}

