#pragma once

#include <QObject>

#include "../PlaceAbstract.h"

class PlaceAdditional : public PlaceAbstract
{
public:
    explicit PlaceAdditional(QObject *parent = nullptr);

    virtual void slotOnCommand(const Message::Ptr &message, const ChatInfo &chatInfo) override;
    virtual void slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatInfo &chatInfo) override;

private:
    void onAdditional(const Message::Ptr &message);
    void onAnyMessage(const Message::Ptr &message);
    void onShowHistory(const CallbackQuery::Ptr &callbackQuery);
    void onDeletePrayerNeed(const CallbackQuery::Ptr &callbackQuery);
    void onDeleteHistory(const CallbackQuery::Ptr &callbackQuery);
    void onDeveloper(const CallbackQuery::Ptr &callbackQuery);
    void onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery);

    std::string getListPrayerNeeds(const Message::Ptr &message);
};

