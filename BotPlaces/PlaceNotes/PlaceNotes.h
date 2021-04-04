#pragma once

#include <QObject>

#include "../PlaceAbstract.h"

class PlaceNotes : public PlaceAbstract
{
public:
    explicit PlaceNotes(QObject *parent = nullptr);

    virtual void slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo) override;
    virtual void slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo) override;

private:
    void onAddNote(const Message::Ptr &message);
    void onRemoveNote(const Message::Ptr &message);
    void onAnyMessage(const Message::Ptr &message);
    void onCreateGroupCallbackQuery(const CallbackQuery::Ptr &callbackQuery);
    void onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery);

    std::string getListNotes(const QString &group, const std::int64_t chat_id);
    std::string getListNotes(const std::string &group, const std::int64_t chat_id);
};

