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

#include "PlaceAdditional.h"

PlaceAdditional::PlaceAdditional(QObject *parent) : PlaceAbstract(parent)
{
    // TODO: new functiono - change_group_position
}

void PlaceAdditional::slotOnCommand(const Message::Ptr &message, const ChatActions &chatActions)
{
    // TODO: replace switch with if/else
    switch (chatActions.currentCommand) {
    case Content::Additional_Additional:
        onAdditional(message);
        break;
    case Content::MultiPlace_AnyMessage:
        onAdditional(message);
        break;
    default:
        PlaceAbstract::slotOnCommand(message, chatActions);
    }
}

void PlaceAdditional::slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatActions &chatActions)
{
    switch (chatActions.currentCommand) {
    case Content::Additional_ShowHistory:
        onShowHistory(callbackQuery);
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
        PlaceAbstract::slotOnCallbackQuery(callbackQuery, chatActions);
    }
}

void PlaceAdditional::onAdditional(const Message::Ptr &message)
{
    static const auto keyboard = createOneColumnInlineKeyboardMarkup({Content::getCommandStr(Content::Additional_ShowHistory)
                                                                      , Content::getCommandStr(Content::Additional_DeleteHistory)
                                                                      , Content::getCommandStr(Content::Additional_Developer)
                                                                     });
    static const auto answer { QObject::tr("Select function:").toStdString() };
    bot->getApi().sendMessage(message->chat->id, answer, false, 0, keyboard, "Markdown");
}

void PlaceAdditional::onAnyMessage(const Message::Ptr &message)
{
    if (QString::fromStdString(message->text).toLower() == "ping") {
        sendMainMenuMessage(message->chat->id, "Pong!");
    }
}

void PlaceAdditional::onShowHistory(const CallbackQuery::Ptr &callbackQuery)
{
//    static const auto answer { QObject::tr("\n\nShow your history:\n").toStdString() };
//    const auto vecNeeds = managerDatabase->getVecNotes(callbackQuery->message->chat->id, ManagerDatabase::asdf);
//    QString needs;
//    for (const auto &prayerNeed: vecNeeds) {
//        needs += QString("%1 (%2)\n").arg(prayerNeed.need, prayerNeed.answer);
//    }
//    bot->getApi().answerCallbackQuery(callbackQuery->id);
//    sendMainMenuMessage(callbackQuery->message->chat->id, answer + needs.toStdString());
}

void PlaceAdditional::onDeleteHistory(const CallbackQuery::Ptr &callbackQuery)
{
//    static const auto answer { QObject::tr("Your all prayer needs has deleted").toStdString() };
//    managerDatabase->deleteAllNotes(callbackQuery->message->chat->id);
//    bot->getApi().answerCallbackQuery(callbackQuery->id);
//    sendMainMenuMessage(callbackQuery->message->chat->id, answer);
}

void PlaceAdditional::onDeveloper(const CallbackQuery::Ptr &callbackQuery)
{
    const auto answer { QObject::tr("Hello! I'm Ilya. Write me if you have any questions (https://t.me/ilyaSongrov)").toStdString() };
    sendMainMenuMessage(callbackQuery->message->chat->id, answer);
}

void PlaceAdditional::onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery)
{
//    const auto chat_id = callbackQuery->message->chat->id;
//    if (chatContainsLastCommand(chat_id, Content::Additional_DeletePrayerNeed)) {
//        static const auto answer { QObject::tr("Prayer need has been successfully removed!").toStdString() };
//        bool ok;
//        const int need_id = QString::fromStdString(callbackQuery->data).toInt(&ok);
//        if (ok) {
//            managerDatabase->deleteAllNotes(need_id, chat_id);
//        }
//        bot->getApi().answerCallbackQuery(callbackQuery->id);
//        sendMainMenuMessage(chat_id, answer);
//    }
}

std::string PlaceAdditional::getListPrayerNeeds(const Message::Ptr &message)
{
//    const QString answer { "List prayers:\n" + managerDatabase->getListNotes(message->chat->id, ManagerDatabase::PrayerNeedsWithAnswerOfGod).join('\n') };
//    return answer.toStdString();
}
