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
#include <QString>

#include "chatsettings.h"
#include "chatactions.h"

struct Note{
    QString note_text;
    int note_id;
};

struct GroupPosition{
    int x = -1;
    int y = -1;
    bool isNull(){ return x == -1 && y == -1; }
};

struct Group{
    QString group_name;
    GroupPosition groupPosition;
    QVector<Note> vecNotes;
};

struct Chat{
    std::int64_t chat_id;
    ChatActions chatActions;
    ChatSettings chatSettings;
    QVector<Group> vecGroups;
};

QDebug operator<< (QDebug dbg, const Note &note);
QDebug operator<< (QDebug dbg, const GroupPosition &groupPosition);
QDebug operator<< (QDebug dbg, const Group &group);
QDebug operator<< (QDebug dbg, const Chat &chat);
