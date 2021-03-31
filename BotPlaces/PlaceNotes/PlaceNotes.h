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
    void onListPrayerNeed(const Message::Ptr &message);
    void onAnyMessage(const Message::Ptr &message);
    void onNewGroupCallbackQuery(const CallbackQuery::Ptr &callbackQuery);
    void onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery);

    std::string getListPrayerNeeds(const Message::Ptr &message);
};

