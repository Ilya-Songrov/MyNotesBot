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

#include "placestart.h"

PlaceStart::PlaceStart(QObject *parent) : PlaceAbstract(parent)
{

}

void PlaceStart::slotOnCommand(const Message::Ptr &message, const ChatActions &chatActions)
{
    if (chatActions.currentCommand == Content::Start_ChooseLanguage) {
        onChooseLanguage(message);
    }
    else{
        PlaceAbstract::slotOnCommand(message, chatActions);
    }
}

void PlaceStart::onChooseLanguage(const Message::Ptr &message)
{
    sendMainMenuButtons(message->chat->id);
}
