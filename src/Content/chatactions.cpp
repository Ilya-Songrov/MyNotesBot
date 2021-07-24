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

#include "chatactions.h"


QDebug operator<< (QDebug dbg, const ChatActions &chatActions)
{
    dbg << "currentPlace   :" << chatActions.currentPlace     << Qt::endl;
    dbg << "currentCommand :" << chatActions.currentCommand   << Qt::endl;
    dbg << "lastPlace      :" << chatActions.lastPlace        << Qt::endl;
    dbg << "lastCommand    :" << chatActions.lastCommand      << Qt::endl;
    dbg << "lastGroup      :" << chatActions.lastGroup        << Qt::endl;
    return dbg;
}
