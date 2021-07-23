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
