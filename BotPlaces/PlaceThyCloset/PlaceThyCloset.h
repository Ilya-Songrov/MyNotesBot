#pragma once

#include <QObject>

#include "../PlaceAbstract.h"

class PlaceThyCloset : public PlaceAbstract
{
public:
    explicit PlaceThyCloset(QObject *parent = nullptr);

    virtual void slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo) override;
    virtual void slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo) override;

private:
    void onAddPrayerNeed(const Message::Ptr &message);
    void onAddAnswerOfGod(const Message::Ptr &message);
    void onListPrayerNeed(const Message::Ptr &message);
    void onAnyMessage(const Message::Ptr &message);
    void onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery);

    std::string getListPrayerNeeds(const Message::Ptr &message);
};

