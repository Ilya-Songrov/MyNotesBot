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

#include "managerdatabase.h"

ManagerDatabase::ManagerDatabase(const QString &pathDatabase, const SettingsQuery settingsQuery, QObject *parent)
    : DatabaseConnector(pathDatabase, parent)
    , sq(settingsQuery)
{
    if (sq == LoadAllSettingsInMemory) {
        loadAllSettings();
    }
}

ManagerDatabase::~ManagerDatabase()
{
    qDebug() << "function:" << __FUNCTION__ << Qt::endl;
}

ChatSettings ManagerDatabase::getChatSettings(const int64_t chat_id)
{
    if (sq == LoadAllSettingsInMemory) {
        return mapAllChatSettings->value(chat_id);
    }
    else if (sq == ReadSettingsFromDatabaseEveryTime) {
        return DatabaseConnector::getChatSettings(chat_id);
    }
    return {};
}

bool ManagerDatabase::updateChatSettings(const int64_t chat_id, const ChatSettings &chatSettings)
{
    if (sq == LoadAllSettingsInMemory) {
        mapAllChatSettings->insert(chat_id, chatSettings);
        DatabaseConnector::updateChatSettings(chat_id, chatSettings);
    }
    else if (sq == ReadSettingsFromDatabaseEveryTime) {
        DatabaseConnector::updateChatSettings(chat_id, chatSettings);
    }
    return false;
}

void ManagerDatabase::loadAllSettings()
{
    mapAllChatSettings = DatabaseConnector::getAllChatSettings();
}
