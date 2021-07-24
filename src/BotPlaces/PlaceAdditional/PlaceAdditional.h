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

#pragma once

#include <QObject>

#include "../PlaceAbstract.h"

class PlaceAdditional : public PlaceAbstract
{
public:
    explicit PlaceAdditional(QObject *parent = nullptr);

    virtual void slotOnCommand(const Message::Ptr &message, const ChatActions &chatActions) override;
    virtual void slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatActions &chatActions) override;

private:
    void onAdditional(const Message::Ptr &message);
    void onAnyMessage(const Message::Ptr &message);
    void onShowHistory(const CallbackQuery::Ptr &callbackQuery);
    void onDeleteHistory(const CallbackQuery::Ptr &callbackQuery);
    void onDeveloper(const CallbackQuery::Ptr &callbackQuery);
    void onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery);

    std::string getListPrayerNeeds(const Message::Ptr &message);
};

