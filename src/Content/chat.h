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
    QString chat_id;
    ChatSettings chatSettings;
    ChatActions chatActions;
    QVector<Group> vecGroups;
};

