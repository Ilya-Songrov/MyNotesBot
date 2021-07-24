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

class PlaceNotes : public PlaceAbstract
{
public:
    explicit PlaceNotes(QObject *parent = nullptr);

    virtual void slotOnCommand(const Message::Ptr &message, const ChatActions &chatActions) override;
    virtual void slotOnCallbackQuery(const CallbackQuery::Ptr &callbackQuery, const ChatActions &chatActions) override;

private:
    void onAddNote(const Message::Ptr &message);
    void onRemoveNote(const Message::Ptr &message);
    void onAnyMessage(const Message::Ptr &message);
    void onCreateGroupCallbackQuery(const CallbackQuery::Ptr &callbackQuery);
    void onAnyCallbackQuery(const CallbackQuery::Ptr &callbackQuery);

    std::string getListNotes(const QString &group, const std::int64_t chat_id);
    std::string getListNotes(const std::string &group, const std::int64_t chat_id);
};

