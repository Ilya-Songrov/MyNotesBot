#include "chatsettings.h"

QDebug operator<<(QDebug dbg, const ChatSettings &chatSettings)
{
    dbg << QString("ChatSettings(language: %1, quantityGroupsX: %2)").arg(chatSettings.language).arg(chatSettings.quantityGroupsX) << Qt::endl;
    return dbg;
}
