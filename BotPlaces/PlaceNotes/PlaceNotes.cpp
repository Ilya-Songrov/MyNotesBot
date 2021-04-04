#include "PlaceNotes.h"

PlaceNotes::PlaceNotes(QObject *parent) : PlaceAbstract(parent)
{

}

void PlaceNotes::slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo)
{
    qDebug() << "function:" << __FUNCTION__ << Qt::endl;
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
        onNewGroupCallbackQuery(callbackQuery);
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
    //    bot->getApi().sendMessage(message->chat->id, getListNotes(message->chat->id), false, 0, getMainMenuButtons(message->chat->id));
}

void PlaceNotes::onAnyMessage(const Message::Ptr &message)
{
    qDebug() << "function:" << __FUNCTION__ << Qt::endl;
    const auto chat_id = message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::Notes_NewGroup)) {
        static const auto answer { QObject::tr("Your group was ").toStdString() };
        const bool ret = managerDatabase->addGroup(message->text, chat_id);
        sendMainMenuMessage(chat_id, answer + (ret ? "created" : "not created"));
    }
    else if (chatContainsLastCommand(chat_id, Content::Notes_GroupWasSelected)) {
        const auto lastGroup = getChatInfo(chat_id).lastGroup;
        if (!lastGroup.isEmpty()) {
            managerDatabase->addNote(message->text, lastGroup, chat_id);
        }
        sendMainMenuMessage(chat_id, getListNotes(lastGroup, chat_id));
    }
    else if (managerDatabase->existsGroup(message->text, chat_id)) {
        static const auto answer { QObject::tr("Your group was ").toStdString() };
        auto chatInfo       = getChatInfo(chat_id);
        chatInfo.lastGroup  = QString::fromStdString(message->text);
        setChatInfo(chat_id, chatInfo);
        sendMainMenuMessage(chat_id, getListNotes(message->text, chat_id));
    }
    else if (QString::fromStdString(message->text).toLower() == "ping") {
        sendMainMenuMessage(chat_id, "Pong!");
    }
    else{
        const auto lastGroup = getChatInfo(chat_id).lastGroup;
        if (!lastGroup.isEmpty()) {
            managerDatabase->addNote(message->text, lastGroup, chat_id);
            sendMainMenuMessage(chat_id, getListNotes(lastGroup, chat_id));
        }
        else{
            sendMainMenuButtons(chat_id);
        }
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
        static const auto answer { QObject::tr("Write your note:").toStdString() };
        const auto group = QString::fromStdString(callbackQuery->data);
        bot->getApi().answerCallbackQuery(callbackQuery->id);
        bot->getApi().sendMessage(chat_id, answer);
        auto chatInfo           = getChatInfo(chat_id);
        chatInfo.currentCommand = Content::Command::Notes_GroupWasSelected;
        chatInfo.lastGroup      = group;
        setChatInfo(chat_id, chatInfo);
    }
}

std::string PlaceNotes::getListNotes(const QString &group, const int64_t chat_id)
{
    return getListNotes(group.toStdString(), chat_id);
}

std::string PlaceNotes::getListNotes(const std::string &group, const int64_t chat_id)
{
    const QString answer { QObject::tr("List notes:") + "\n" + managerDatabase->getListNotes(group, chat_id).join('\n') };
    return answer.toStdString();
}

