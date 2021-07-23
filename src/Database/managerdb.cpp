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

#include "managerdb.h"

ManagerDb::ManagerDb(const QString &pathDatabase, QObject *parent)
    : DatabaseConnector(pathDatabase, parent)
{
    // TODO: "◾️ " + note
}

ManagerDb::~ManagerDb()
{
    qDebug() << "function:" << __FUNCTION__ << Qt::endl;
}

bool ManagerDb::addGroup(const QString &group_name, const int64_t chat_id)
{
    return DatabaseConnector::addOnlyGroup(group_name, getNextGroupPosition(chat_id), chat_id);
}

bool ManagerDb::addGroup(const std::string &group_name, const int64_t chat_id)
{
    return addGroup(QString::fromStdString(group_name), chat_id);
}

bool ManagerDb::deleteGroup(const QString &group_name, const int64_t chat_id)
{
    const bool ret1 = DatabaseConnector::deleteOnlyGroup(group_name, chat_id);
    const bool ret2 = DatabaseConnector::deleteAllNotes(group_name, chat_id);
    return ret1 && ret2;
}

bool ManagerDb::deleteGroup(const std::string &group, const int64_t chat_id)
{
    return deleteGroup(QString::fromStdString(group), chat_id);
}

QStringList ManagerDb::getListNotes(const std::string &group, const int64_t chat_id)
{
    QStringList list;
    const auto vec = DatabaseConnector::getVecNotes(QString::fromStdString(group), chat_id);
    for (const auto &note: vec) {
        list.append(note.note_text);
    }
    return list;
}

QStringList ManagerDb::getListGroups(const int64_t chat_id)
{
    QStringList list;
    const auto vec = DatabaseConnector::getVecGroups(chat_id);
    for (const auto &group: vec) {
        list.append(group.group_name);
    }
    return list;
}

QVector<QStringList> ManagerDb::getLayoutsGroups(const int64_t chat_id)
{
    const auto getSortingGroupName = [](QList<QPair<QString, int> > &listTemp)->QStringList{
        std::sort(listTemp.begin(), listTemp.end(), [](const QPair<QString, int> a, QPair<QString, int> b){ return a.second < b.second; });
        QStringList list;
        for (const auto &pair: listTemp) {
            list.append(pair.first);
        }
        return list;
    };

    const auto vec = DatabaseConnector::getVecGroups(chat_id);
    QVector<QStringList> vecLayouts;
    QList<QPair<QString, int> > listTemp;
    int lastPositionY = 0;
    for (const auto &group: vec) {
        if (group.groupPosition.y > lastPositionY) {
            const QStringList list = getSortingGroupName(listTemp);
            if (!list.isEmpty()) {
                vecLayouts.append(list);
            }
            listTemp.clear();
        }
        listTemp.append(qMakePair(group.group_name, group.groupPosition.x));
        lastPositionY = group.groupPosition.y;
    }
    const QStringList list = getSortingGroupName(listTemp);
    if (!list.isEmpty()) {
        vecLayouts.append(list);
    }
    return vecLayouts;
}

GroupPosition ManagerDb::getNextGroupPosition(const int64_t chat_id)
{
    const ChatSettings chatSettings = getChatSettings(chat_id);
    GroupPosition groupPositionMax = getCurrentMaxGroupPosition(chat_id);
    if (groupPositionMax.isNull() || groupPositionMax.x < 0 || groupPositionMax.y < 0) {
        groupPositionMax = GroupPosition{0,0};
    }
    else if (groupPositionMax.x >= chatSettings.quantityGroupsX) {
        ++groupPositionMax.y;
        groupPositionMax.x = 0;
    }
    else{
        ++groupPositionMax.x;
    }
    return groupPositionMax;
}

GroupPosition ManagerDb::getCurrentMaxGroupPosition(const int64_t chat_id)
{
    const auto vec = getVecGroups(chat_id);
    GroupPosition groupPositionMax = GroupPosition{0,0};
    for (const auto &group: vec) {
        if (group.groupPosition.y > groupPositionMax.y) {
            groupPositionMax.y = group.groupPosition.y;
            groupPositionMax.x = 0;
        }
        if (group.groupPosition.y == groupPositionMax.y) {
            groupPositionMax.x = group.groupPosition.x > groupPositionMax.y ? group.groupPosition.x : groupPositionMax.y;
        }
    }
    return groupPositionMax;
}
