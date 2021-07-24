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

#include "chat.h"

QDebug operator<< (QDebug dbg, const Note &note)
{
    dbg << QString("Note(note_text: %1, note_id: %2)").arg(note.note_text).arg(note.note_id);
    return dbg;
}
QDebug operator<< (QDebug dbg, const GroupPosition &groupPosition)
{
    dbg << QString("GroupPosition(x: %1, y: %2)").arg(groupPosition.x).arg(groupPosition.y);
    return dbg;
}
QDebug operator<< (QDebug dbg, const Group &group)
{
    dbg << "group_name: " + group.group_name << Qt::endl;
    dbg << group.groupPosition << Qt::endl;
    for (const auto &note : group.vecNotes) {
        dbg << note << Qt::endl;
    }
    return dbg;
}
QDebug operator<< (QDebug dbg, const Chat &chat)
{
    dbg << "*** chat_id: "      << chat.chat_id << Qt::endl;
    dbg << "*** ChatActions:"   << Qt::endl << chat.chatActions << Qt::endl;
    dbg << "*** ChatSettings:"  << Qt::endl << chat.chatSettings << Qt::endl;
    dbg << "*** Groups:"        << Qt::endl;
    for (const auto &group : chat.vecGroups) {
        dbg << group;
    }
    return dbg;
}
