#pragma once

#include <QObject>
#include <QDebug>
#include <QtSql>

#include "../Content/Content.h"
#include "../Content/ChatInfo.h"
#include "../GlobalData/GlobalData.h"

#include <tgbot/tgbot.h>
using namespace TgBot;

class PlaceAbstract : public QObject
{
    Q_OBJECT
public:
    explicit PlaceAbstract(QObject *parent = nullptr);

    static void initMapAllChats(std::shared_ptr<QMap<std::uint64_t, ChatInfo> > mapAllChatsPtr);

    virtual void slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo);
    virtual void slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo);

protected:
    ReplyKeyboardMarkup::Ptr createOneColumnReplyKeyboardMarkup(const QStringList &listButtons, const bool resizeKeyboard = true, const bool oneTimeKeyboard = false);
    ReplyKeyboardMarkup::Ptr createReplyKeyboardMarkup(const QVector<QStringList> &vecLayouts, const bool resizeKeyboard = true, const bool oneTimeKeyboard = false);

    InlineKeyboardMarkup::Ptr createOneColumnInlineKeyboardMarkup(const QStringList &listButtons);
    InlineKeyboardMarkup::Ptr createOneColumnInlineKeyboardMarkup(const QList<QPair<QString, QString> > &listButtonsNameData);
    InlineKeyboardMarkup::Ptr createInlineKeyboardMarkup(const QVector<QStringList> &vecLayouts);

    ReplyKeyboardMarkup::Ptr getMainMenuButtons(const int64_t chat_id);
    void sendMainMenuButtons(const std::int64_t chat_id);
    void sendMainMenuMessage(const std::int64_t chat_id, const std::string &message);
    void sendInlineKeyboardMarkupMessage(const std::int64_t chat_id, const std::string &message, const InlineKeyboardMarkup::Ptr inlineKeyboardMarkup);

    inline void setChatInfo(const std::int64_t chat_id, const ChatInfo &chatInfo){ mapAllChats->insert(chat_id, chatInfo); }
    inline ChatInfo getChatInfo(const std::int64_t chat_id){ return mapAllChats->value(chat_id); }
    void updateCahtInfoLastGroup(const std::int64_t chat_id, const QString &lastGroup);
    void updateCahtInfoCurrentCommand(const std::int64_t chat_id, const Content::Command currentCommand);
    void updateCahtInfoCurrentCommandAndLastGroup(const std::int64_t chat_id, const Content::Command currentCommand, const QString &lastGroup);

    inline bool chatContainsLastCommand(const std::int64_t chat_id, const Content::Command command){ return getChatInfo(chat_id).lastCommand == command; }

protected:
    static std::shared_ptr<QMap<std::uint64_t, ChatInfo> > mapAllChats;
};

