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
        sendMainMenuButtons(message->chat->id);
        break;
    case Content::MultiPlace_Help:
        sendMainMenuButtons(message->chat->id);
        break;
    case Content::MultiPlace_AnyMessage:
        if (QString::fromStdString(message->text).toLower() == "ping") {
            sendMainMenuMessage(message->chat->id, "Pong!");
        }
        break;
    default:
        static const auto answer { QObject::tr("Query is not correct").toStdString() };
        sendMainMenuMessage(message->chat->id, answer);
    }
}

void PlaceAbstract::slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo)
{
    switch (chatInfo.currentCommand) {
    case Content::MultiPlace_Start:
        sendMainMenuButtons(callbackQuery->message->chat->id);
        break;
    case Content::MultiPlace_Help:
        sendMainMenuButtons(callbackQuery->message->chat->id);
        break;
    case Content::MultiPlace_AnyMessage:
        if (QString::fromStdString(callbackQuery->message->text).toLower() == "ping") {
            sendMainMenuMessage(callbackQuery->message->chat->id, "Pong!");
        }
        break;
    default:
        static const auto answer { QObject::tr("Query is not correct").toStdString() };
        sendMainMenuMessage(callbackQuery->message->chat->id, answer);
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

ReplyKeyboardMarkup::Ptr PlaceAbstract::getMainMenuButtons(const int64_t chat_id)
{
    static const QStringList buttonsAddRemove = { Content::getCommandStr(Content::Notes_AddNote), Content::getCommandStr(Content::Notes_RemoveNote) };
    const auto listGroups = managerDatabase->getListGroups(chat_id);
    QVector<QStringList> vecLayouts;
    QStringList listTemp;
    for (int var = listGroups.size() - 1; var >= 0; --var) {
        if ((var + 1) % 4 == 0 && !listTemp.isEmpty()) {
            vecLayouts.append(listTemp);
            listTemp.clear();
        }
        listTemp.insert(0, listGroups.at(var));
    }
    if (!listTemp.isEmpty()) {
        vecLayouts.push_back(listTemp);
    }
    vecLayouts.append(buttonsAddRemove);
    const auto kb = createReplyKeyboardMarkup(vecLayouts, true, false);
    return kb;
}

void PlaceAbstract::sendMainMenuButtons(const int64_t chat_id)
{
    static const auto answer { QObject::tr("Hello. This is note bot. You can create anonymous notes. \n\nMay God bless you.").toStdString() };
    bot->getApi().sendMessage(chat_id, answer, false, 0, getMainMenuButtons(chat_id));
}

void PlaceAbstract::sendMainMenuMessage(const int64_t chat_id, const std::string &message)
{
    bot->getApi().sendMessage(chat_id, message, false, 0, getMainMenuButtons(chat_id));
}

void PlaceAbstract::sendInlineKeyboardMarkupMessage(const int64_t chat_id, const std::string &message, const InlineKeyboardMarkup::Ptr inlineKeyboardMarkup)
{
    bot->getApi().sendMessage(chat_id, message, false, 0, inlineKeyboardMarkup);
}
