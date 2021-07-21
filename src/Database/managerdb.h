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

#include "databaseconnector.h"
#include "chatsettings.h"

class ManagerDb : public DatabaseConnector
{
public:
    enum SettingsQuery{
        LoadAllSettingsInMemory,
        ReadSettingsFromDatabaseEveryTime,
    };
    explicit ManagerDb(const QString &pathDatabase, const SettingsQuery settingsQuery = LoadAllSettingsInMemory, QObject *parent = nullptr);
    virtual ~ManagerDb();

    bool addGroup(const QString &group_name, const std::int64_t chat_id);
    bool addGroup(const std::string &group_name, const std::int64_t chat_id);

    bool deleteGroup(const QString &group_name, const std::int64_t chat_id);
    bool deleteGroup(const std::string &group_name, const std::int64_t chat_id);

    QStringList getListNotes(const std::string &group, const std::int64_t chat_id);

    QStringList getListGroups(const std::int64_t chat_id);
    QVector<QStringList> getLayoutsGroups(const std::int64_t chat_id);

    virtual ChatSettings getChatSettings(const std::int64_t chat_id);
    virtual bool updateChatSettings(const std::int64_t chat_id, const ChatSettings &chatSettings);

private:
    void loadAllSettings();
    GroupPosition getNextGroupPosition(const std::int64_t chat_id);
    GroupPosition getCurrentMaxGroupPosition(const std::int64_t chat_id);


private:
    const SettingsQuery sq;
    std::shared_ptr<QMap<std::uint64_t, ChatSettings> > mapAllChatSettings;
};

