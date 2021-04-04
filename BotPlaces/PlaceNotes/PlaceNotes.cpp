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
    if (chatInfo.currentCommand == Content::Notes_CreateGroup) {
        onCreateGroupCallbackQuery(callbackQuery);
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
    static const auto createGroup = Content::getCommandStr(Content::Notes_CreateGroup);
    auto list = managerDatabase->getListGroups(message->chat->id);
    list.append(createGroup);
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

    const auto chat_id = message->chat->id;
    static const auto answer { QObject::tr("Select group:").toStdString() };
    const auto list = managerDatabase->getListGroups(chat_id);
    const auto inlineButtonPrayerNeed = createOneColumnInlineKeyboardMarkup(list);
    sendInlineKeyboardMarkupMessage(chat_id, answer, inlineButtonPrayerNeed);
}
//void PlaceAdditional::onDeletePrayerNeed(const CallbackQuery::Ptr &callbackQuery)
//{
//    static const auto answer { QObject::tr("Select the prayer need:").toStdString() };
//    const auto vecNeeds = managerDatabase->getVecPrayerNeeds(callbackQuery->message->chat->id);
//    QList<QPair<QString, QString> > listButtons;
//    for (const auto &prayerNeed: vecNeeds) {
//        listButtons.append(qMakePair(prayerNeed.need.left(15) + " ...", prayerNeed.need_id));
//    }
//    const auto inlineButtonPrayerNeed = createOneColumnInlineKeyboardMarkup(listButtons);
//    bot->getApi().answerCallbackQuery(callbackQuery->id);
//    sendInlineKeyboardMarkupMessage(callbackQuery->message->chat->id, answer, inlineButtonPrayerNeed);
//}
//const auto chat_id = callbackQuery->message->chat->id;
//if (chatContainsLastCommand(chat_id, Content::Additional_DeletePrayerNeed)) {
//    static const auto answer { QObject::tr("Prayer need has been successfully removed!").toStdString() };
//    bool ok;
//    const int need_id = QString::fromStdString(callbackQuery->data).toInt(&ok);
//    if (ok) {
//        managerDatabase->deletePrayerNeed(need_id, chat_id);
//    }
//    bot->getApi().answerCallbackQuery(callbackQuery->id);
//    sendStartingMessage(chat_id, answer);
//}

void PlaceNotes::onAnyMessage(const Message::Ptr &message)
{
    const auto chat_id = message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::Notes_CreateGroup)) {
        static const auto answer { QObject::tr("Your group was ").toStdString() };
        const bool ret = managerDatabase->addGroup(message->text, chat_id);
        sendMainMenuMessage(chat_id, answer + (ret ? "created" : "not created"));
    }
    else if (chatContainsLastCommand(chat_id, Content::Notes_GroupWasSelectedToAdd)) {
        const auto lastGroup = getChatInfo(chat_id).lastGroup;
        if (!lastGroup.isEmpty()) {
            managerDatabase->addNote(message->text, lastGroup, chat_id);
        }
        sendMainMenuMessage(chat_id, getListNotes(lastGroup, chat_id));
    }
    else if (managerDatabase->existsGroup(message->text, chat_id)) {
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

void PlaceNotes::onCreateGroupCallbackQuery(const CallbackQuery::Ptr &callbackQuery)
{
    static const auto answer { QObject::tr("Write name of group:").toStdString() };
    const auto chat_id = callbackQuery->message->chat->id;
    bot->getApi().answerCallbackQuery(callbackQuery->id);
    bot->getApi().sendMessage(chat_id, answer);
}

void PlaceNotes::onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery)
{
    static const auto removeGroup = Content::getCommandStr(Content::Notes_RemoveGroup);
    const auto chat_id = callbackQuery->message->chat->id;
    const auto data = QString::fromStdString(callbackQuery->data);
    if (chatContainsLastCommand(chat_id, Content::Notes_AddNote)) {
        static const auto answer { QObject::tr("Write your note:").toStdString() };
        auto chatInfo           = getChatInfo(chat_id);
        chatInfo.currentCommand = Content::Command::Notes_GroupWasSelectedToAdd;
        chatInfo.lastGroup      = data;
        setChatInfo(chat_id, chatInfo);
        bot->getApi().answerCallbackQuery(callbackQuery->id);
        bot->getApi().sendMessage(chat_id, answer);
    }
    else if (chatContainsLastCommand(chat_id, Content::Notes_RemoveNote)) {
        static const auto answer { QObject::tr("Select the note:").toStdString() };
        const auto vecNotes = managerDatabase->getVecNotes(data, chat_id);
        QList<QPair<QString, QString> > listButtons;
        for (const auto &note: vecNotes) {
            listButtons.append(qMakePair(note.note.left(15) + " ...", note.note_id));
        }
        listButtons.append(qMakePair(removeGroup, data));
        const auto inlineButtonPrayerNeed = createOneColumnInlineKeyboardMarkup(listButtons);
        auto chatInfo           = getChatInfo(chat_id);
        chatInfo.currentCommand = Content::Command::Notes_GroupWasSelectedToRemove;
        chatInfo.lastGroup      = data;
        setChatInfo(chat_id, chatInfo);
        bot->getApi().answerCallbackQuery(callbackQuery->id);
        sendInlineKeyboardMarkupMessage(callbackQuery->message->chat->id, answer, inlineButtonPrayerNeed);
    }
    else if (chatContainsLastCommand(chat_id, Content::Notes_GroupWasSelectedToRemove)) {
        bool ret = false;
        std::string answer;
        qDebug() << "datadata" << data << Qt::endl;
        if (data == removeGroup) {
            const QString group = data;
            ret = managerDatabase->deleteAllNotes(group, chat_id);
            answer = QObject::tr("Group has been %1 removed!").arg(ret ? "" : "not").toStdString();
        }
        else{
            bool ok;
            const int note_id = QString::fromStdString(callbackQuery->data).toInt(&ok);
            if (ok) {
                ret = managerDatabase->deleteAllNotes(note_id, chat_id);
            }
            answer = QObject::tr("Note has been %1 removed!").arg(ret ? "" : "not").toStdString();
        }
        bot->getApi().answerCallbackQuery(callbackQuery->id);
        sendMainMenuMessage(chat_id, answer);
    }
}

std::string PlaceNotes::getListNotes(const QString &group, const int64_t chat_id)
{
    return getListNotes(group.toStdString(), chat_id);
}

std::string PlaceNotes::getListNotes(const std::string &group, const int64_t chat_id)
{
    const QString answer { QObject::tr("List notes") + QString(" (%1):\n").arg(group.c_str()) + managerDatabase->getListNotes(group, chat_id).join('\n') };
    return answer.toStdString();
}

