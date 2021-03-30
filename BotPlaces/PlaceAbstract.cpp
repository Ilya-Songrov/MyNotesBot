#include "PlaceAbstract.h"

std::shared_ptr<QMap<std::uint64_t, ChatInfo> > PlaceAbstract::mapAllChats;

PlaceAbstract::PlaceAbstract(QObject *parent) : QObject(parent)
{

}

void PlaceAbstract::initMapAllChats(std::shared_ptr<QMap<uint64_t, ChatInfo> > mapAllChatsPtr)
{
    mapAllChats = mapAllChatsPtr;
}

void PlaceAbstract::slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::MultiPlace_Start:
        sendStartingButtons(message->chat->id);
        break;
    case Content::MultiPlace_Help:
        sendStartingButtons(message->chat->id);
        break;
    case Content::MultiPlace_AnyMessage:
        if (QString::fromStdString(message->text).toLower() == "ping") {
            sendStartingMessage(message->chat->id, "Pong!");
        }
        break;
    default:
        static const auto answer { QObject::tr("Query is not correct").toStdString() };
        sendStartingMessage(message->chat->id, answer);
    }
}

void PlaceAbstract::slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::MultiPlace_Start:
        sendStartingButtons(callbackQuery->message->chat->id);
        break;
    case Content::MultiPlace_Help:
        sendStartingButtons(callbackQuery->message->chat->id);
        break;
    case Content::MultiPlace_AnyMessage:
        if (QString::fromStdString(callbackQuery->message->text).toLower() == "ping") {
            sendStartingMessage(callbackQuery->message->chat->id, "Pong!");
        }
        break;
    default:
        static const auto answer { QObject::tr("Query is not correct").toStdString() };
        sendStartingMessage(callbackQuery->message->chat->id, answer);
    }
}

ReplyKeyboardMarkup::Ptr PlaceAbstract::createOneColumnReplyKeyboardMarkup(const QStringList &listButtons, const bool resizeKeyboard, const bool oneTimeKeyboard)
{
    ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
    for (const QString &textButton : listButtons) {
        std::vector<KeyboardButton::Ptr> row;
        KeyboardButton::Ptr button(new KeyboardButton);
        button->text = textButton.toStdString();
        row.push_back(button);
        kb->keyboard.push_back(row);
    }
    kb->resizeKeyboard = resizeKeyboard;
    kb->oneTimeKeyboard = oneTimeKeyboard;
    return kb;
}

ReplyKeyboardMarkup::Ptr PlaceAbstract::createReplyKeyboardMarkup(const QVector<QStringList> &vecLayouts, const bool resizeKeyboard, const bool oneTimeKeyboard)
{
    ReplyKeyboardMarkup::Ptr kb(new ReplyKeyboardMarkup);
    for (int var = 0; var < vecLayouts.size(); ++var) {
        std::vector<KeyboardButton::Ptr> row;
        for (const QString &name: vecLayouts.at(var)) {
            KeyboardButton::Ptr button(new KeyboardButton);
            button->text = name.toStdString();
            row.push_back(button);
        }
        kb->keyboard.push_back(row);
    }
    kb->resizeKeyboard = resizeKeyboard;
    kb->oneTimeKeyboard = oneTimeKeyboard;
    return kb;
}

InlineKeyboardMarkup::Ptr PlaceAbstract::createOneColumnInlineKeyboardMarkup(const QStringList &listButtons)
{
    InlineKeyboardMarkup::Ptr kb(new InlineKeyboardMarkup);
    for (const QString &textButton : listButtons) {
        std::vector<InlineKeyboardButton::Ptr> row;
        InlineKeyboardButton::Ptr button(new InlineKeyboardButton);
        button->text = textButton.toStdString();
        button->callbackData = button->text;
        row.push_back(button);
        kb->inlineKeyboard.push_back(row);
    }
    return kb;
}

InlineKeyboardMarkup::Ptr PlaceAbstract::createOneColumnInlineKeyboardMarkup(const QList<QPair<QString, QString> > &listButtonsNameData)
{
    InlineKeyboardMarkup::Ptr kb(new InlineKeyboardMarkup);
    for (const auto &pair : listButtonsNameData) {
        std::vector<InlineKeyboardButton::Ptr> row;
        InlineKeyboardButton::Ptr button(new InlineKeyboardButton);
        button->text = pair.first.toStdString();
        button->callbackData = pair.second.toStdString();
        row.push_back(button);
        kb->inlineKeyboard.push_back(row);
    }
    return kb;
}

InlineKeyboardMarkup::Ptr PlaceAbstract::createInlineKeyboardMarkup(const QVector<QStringList> &vecLayouts)
{
    InlineKeyboardMarkup::Ptr kb(new InlineKeyboardMarkup);
    for (int var = 0; var < vecLayouts.size(); ++var) {
        std::vector<InlineKeyboardButton::Ptr> row;
        for (const QString &name: vecLayouts.at(var)) {
            InlineKeyboardButton::Ptr button(new InlineKeyboardButton);
            button->text = name.toStdString();
            button->callbackData = button->text;
            row.push_back(button);
        }
        kb->inlineKeyboard.push_back(row);
    }
    return kb;
}

ReplyKeyboardMarkup::Ptr PlaceAbstract::getStartingButtons()
{
    static const QStringList layout { Content::getCommandStr(Content::ThyCloset_AddPrayerNeed)
                , Content::getCommandStr(Content::ThyCloset_AddAnswerOfGod)
                , Content::getCommandStr(Content::ThyCloset_ListPrayerNeed)
                , Content::getCommandStr(Content::Additional_Additional)
                                    };
    static const auto kb = createOneColumnReplyKeyboardMarkup(layout, true, true);
    return kb;
}

void PlaceAbstract::sendStartingButtons(const int64_t chat_id)
{
    static const QString answer { QObject::tr("Hello child of God. This bot is designed to make your prayer life effective. \n\nMay God bless you.") };
    static const auto answerStdStr { answer.toStdString() };
    bot->getApi().sendMessage(chat_id, answerStdStr, false, 0, getStartingButtons());
}

void PlaceAbstract::sendStartingMessage(const int64_t chat_id, const std::string &message)
{
    bot->getApi().sendMessage(chat_id, message, false, 0, getStartingButtons());
}

void PlaceAbstract::sendInlineKeyboardMarkupMessage(const int64_t chat_id, const std::string &message, const InlineKeyboardMarkup::Ptr inlineKeyboardMarkup)
{
    bot->getApi().sendMessage(chat_id, message, false, 0, inlineKeyboardMarkup);
}
