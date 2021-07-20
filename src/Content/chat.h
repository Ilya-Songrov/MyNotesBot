#pragma once

#include <QObject>
#include <QString>

#include "chatsettings.h"
#include "chatactions.h"

struct Note{
    QString note;
};

struct Group{
    QString group;
    int groupPositionX;
    int groupPositionY;
    QVector<Note> vecNotes;
};

struct Chat{
    QString chat_id;
    ChatSettings chatSettings;
    ChatActions chatActions;
    QVector<Group> vecGroups;
};

