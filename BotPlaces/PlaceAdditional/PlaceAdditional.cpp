#include "PlaceAdditional.h"

PlaceAdditional::PlaceAdditional(QObject *parent) : PlaceAbstract(parent)
{

}

void PlaceAdditional::slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::Additional_Additional:
        onAdditional(message);
        break;
    case Content::MultiPlace_AnyMessage:
        onAdditional(message);
        break;
    default:
        PlaceAbstract::slotOnCommand(message, chatInfo);
    }
}

void PlaceAdditional::slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::Additional_ShowHistory:
        onShowHistory(callbackQuery);
        break;
    case Content::Additional_DeletePrayerNeed:
        onDeletePrayerNeed(callbackQuery);
        break;
    case Content::Additional_DeleteHistory:
        onDeleteHistory(callbackQuery);
        break;
    case Content::Additional_Developer:
        onDeveloper(callbackQuery);
        break;
    case Content::MultiPlace_AnyCallbackQuery:
        onAnyCallbackQuery(callbackQuery);
        break;
    default:
        PlaceAbstract::slotOnCallbackQuery(callbackQuery, chatInfo);
    }
}

void PlaceAdditional::onAdditional(const Message::Ptr &message)
{
    static const auto keyboard = createOneColumnInlineKeyboardMarkup({Content::getCommandStr(Content::Additional_ShowHistory)
                                                                      , Content::getCommandStr(Content::Additional_DeletePrayerNeed)
                                                                      , Content::getCommandStr(Content::Additional_DeleteHistory)
                                                                      , Content::getCommandStr(Content::Additional_Developer)
                                                                     });
    static const auto answer { QObject::tr("Select function:").toStdString() };
    bot->getApi().sendMessage(message->chat->id, answer, false, 0, keyboard, "Markdown");
}

void PlaceAdditional::onAnyMessage(const Message::Ptr &message)
{
    if (QString::fromStdString(message->text).toLower() == "ping") {
        sendStartingMessage(message->chat->id, "Pong!");
    }
}

void PlaceAdditional::onShowHistory(const CallbackQuery::Ptr &callbackQuery)
{
    static const auto answer { QObject::tr("\n\nShow your history:\n").toStdString() };
    const auto vecNeeds = managerDatabase->getVecPrayerNeeds(callbackQuery->message->chat->id, ManagerDatabase::PrayerNeedsAll);
    QString needs;
    for (const auto &prayerNeed: vecNeeds) {
        needs += QString("%1 (%2)\n").arg(prayerNeed.need, prayerNeed.answer);
    }
    bot->getApi().answerCallbackQuery(callbackQuery->id);
    sendStartingMessage(callbackQuery->message->chat->id, answer + needs.toStdString());
}

void PlaceAdditional::onDeletePrayerNeed(const CallbackQuery::Ptr &callbackQuery)
{
    static const auto answer { QObject::tr("Select the prayer need:").toStdString() };
    const auto vecNeeds = managerDatabase->getVecPrayerNeeds(callbackQuery->message->chat->id);
    QList<QPair<QString, QString> > listButtons;
    for (const auto &prayerNeed: vecNeeds) {
        listButtons.append(qMakePair(prayerNeed.need.left(15) + " ...", prayerNeed.need_id));
    }
    const auto inlineButtonPrayerNeed = createOneColumnInlineKeyboardMarkup(listButtons);
    bot->getApi().answerCallbackQuery(callbackQuery->id);
    sendInlineKeyboardMarkupMessage(callbackQuery->message->chat->id, answer, inlineButtonPrayerNeed);
}

void PlaceAdditional::onDeleteHistory(const CallbackQuery::Ptr &callbackQuery)
{
    static const auto answer { QObject::tr("Your all prayer needs has deleted").toStdString() };
    managerDatabase->deleteAllPrayerNeeds(callbackQuery->message->chat->id);
    bot->getApi().answerCallbackQuery(callbackQuery->id);
    sendStartingMessage(callbackQuery->message->chat->id, answer);
}

void PlaceAdditional::onDeveloper(const CallbackQuery::Ptr &callbackQuery)
{
    const auto answer { QObject::tr("Hello! I'm Ilya. Write me if you have any questions (https://t.me/ilyaSongrov)").toStdString() };
    sendStartingMessage(callbackQuery->message->chat->id, answer);
}

void PlaceAdditional::onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery)
{
    const auto chat_id = callbackQuery->message->chat->id;
    if (chatContainsLastCommand(chat_id, Content::Additional_DeletePrayerNeed)) {
        static const auto answer { QObject::tr("Prayer need has been successfully removed!").toStdString() };
        bool ok;
        const int need_id = QString::fromStdString(callbackQuery->data).toInt(&ok);
        if (ok) {
            managerDatabase->deletePrayerNeed(need_id, chat_id);
        }
        bot->getApi().answerCallbackQuery(callbackQuery->id);
        sendStartingMessage(chat_id, answer);
    }
}

std::string PlaceAdditional::getListPrayerNeeds(const Message::Ptr &message)
{
    const QString answer { "List prayers:\n" + managerDatabase->getListPrayerNeeds(message->chat->id, ManagerDatabase::PrayerNeedsWithAnswerOfGod).join('\n') };
    return answer.toStdString();
}
