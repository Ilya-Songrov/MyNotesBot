#include "chat.h"

QDebug operator<< (QDebug dbg, const Note &note)
{
    dbg << QString("Note(note_text: %1, note_id: %2)").arg(note.note_text).arg(note.note_id);
    return dbg;
}
QDebug operator<< (QDebug dbg, const GroupPosition &groupPosition)
{
    dbg << QString("GroupPosition(x: %1, y: %2)").arg(groupPosition.x, groupPosition.y);
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
