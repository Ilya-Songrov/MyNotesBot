#include "databaseconnector.h"

DatabaseConnector::DatabaseConnector(const QString &pathDatabase, QObject *parent) : QObject(parent)
{
    openDb(pathDatabase);
    createAllTables();
}

DatabaseConnector::~DatabaseConnector()
{
    qDebug() << "function:" << __FUNCTION__ << Qt::endl;
    if (db.isOpen()){
        db.close();
    }
}

bool DatabaseConnector::isOpen() const
{
    return db.isOpen();
}

bool DatabaseConnector::addNote(const QString &note_text, const QString &group_name, const int64_t chat_id)
{
    if (note_text.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO user_notes (note_text, group_name, chat_id) VALUES (:note_text, :group_name, :chat_id)");
    query.bindValue(":note_text", note_text);
    query.bindValue(":group_name", group_name);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::addNote(const std::string &note_text, const QString &group_name, const int64_t chat_id)
{
    return addNote(QString::fromStdString(note_text), group_name, chat_id);
}

bool DatabaseConnector::replaceNote(const QString &newNote, const QString &oldNote, const QString &group_name, const int64_t chat_id)
{
    if (newNote.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("UPDATE user_notes SET note_text = :oldNote WHERE note_text = :newNote AND group_name = :group_name AND chat_id = :chat_id");
    query.bindValue(":oldNote", oldNote);
    query.bindValue(":newNote", newNote);
    query.bindValue(":group_name", group_name);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::deleteNoteViaID(const int note_id, const int64_t chat_id)
{
    if (note_id < 0){
        qWarning() << __FUNCTION__ << "failed: value cannot be < 0" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM user_notes WHERE (note_id = :note_id AND chat_id = :chat_id)");
    query.bindValue(":note_id", note_id);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::existsGroup(const QString &group_name, const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM user_groups WHERE group_name = :group_name AND chat_id = :chat_id");
    query.bindValue(":group_name", group_name);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if (query.exec()){
        return query.next();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::existsGroup(const std::string &group_name, const int64_t chat_id)
{
    return existsGroup(QString::fromStdString(group_name), chat_id);
}

Chat DatabaseConnector::getChat(const int64_t chat_id) const
{
    Chat chat;
    chat.chat_id        = chat_id;
    chat.chatSettings   = getChatSettings(chat_id);
    chat.chatActions    = getChatActions(chat_id);
    chat.vecGroups      = getVecGroups(chat_id);
    return chat;
}

QVector<Group> DatabaseConnector::getVecGroups(const int64_t chat_id) const
{
    QVector<Group> vec;
    QSqlQuery query;
    query.prepare("SELECT * FROM user_groups WHERE chat_id = :chat_id");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int idGroupName       = record.indexOf("group_name");
    const int idGroupPositionX  = record.indexOf("groupPositionX");
    const int idGroupPositionY  = record.indexOf("groupPositionY");
    while (query.next())
    {
        const QString strGroupName = query.value(idGroupName).toString();
        if (strGroupName.isEmpty()) {
            continue;
        }
        bool okX, okY;
        Group group;
        group.group_name        = strGroupName;
        group.groupPosition.x   = query.value(idGroupPositionX).toInt(&okX);
        group.groupPosition.y   = query.value(idGroupPositionY).toInt(&okY);
        group.vecNotes          = getVecNotes(group.group_name, chat_id);
        vec.append(group);
        if (!okX || !okY) {
            qWarning() << "Can not convert query to number" << __FUNCTION__ << "chat_id" << chat_id << Qt::endl;
        }
    }
    return vec;
}

QVector<Note> DatabaseConnector::getVecNotes(const QString &group_name, const int64_t chat_id) const
{
    QVector<Note> vec;
    QSqlQuery query;
    query.prepare("SELECT * FROM user_notes WHERE group_name = :group_name AND chat_id = :chat_id");
    query.bindValue(":group_name", group_name);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int idNoteText    = record.indexOf("note_text");
    const int idNoteId      = record.indexOf("note_id");
    while (query.next())
    {
        const QString strNoteText = query.value(idNoteText).toString();
        if (strNoteText.isEmpty()) {
            continue;
        }
        bool ok;
        Note note;
        note.note_text      = strNoteText;
        note.note_id        = query.value(idNoteId).toInt(&ok);
        vec.append(note);
        if (!ok) {
            qWarning() << "Can not convert query to number" << __FUNCTION__ << "chat_id" << chat_id << Qt::endl;
        }
    }
    return vec;
}

bool DatabaseConnector::setChatActions(const ChatActions &chatActions, const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO user_chat_actions (chat_id, currentPlace, currentCommand, lastPlace, lastCommand, lastGroup) "
                    "VALUES (:chat_id, :currentPlace, :currentCommand, :lastPlace, :lastCommand, :lastGroup)");
    query.bindValue(":currentPlace"     , chatActions.currentPlace);
    query.bindValue(":currentCommand"   , chatActions.currentCommand);
    query.bindValue(":lastPlace"        , chatActions.lastPlace);
    query.bindValue(":lastCommand"      , chatActions.lastCommand);
    query.bindValue(":lastGroup"        , chatActions.lastGroup);
    query.bindValue(":chat_id"          , varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

ChatActions DatabaseConnector::getChatActions(const int64_t chat_id) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM user_chat_actions WHERE chat_id = :chat_id");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int id_chat_id = record.indexOf("chat_id");
    const int id_currentPlace = record.indexOf("currentPlace");
    const int id_currentCommand = record.indexOf("currentCommand");
    const int id_lastPlace = record.indexOf("lastPlace");
    const int id_lastCommand = record.indexOf("lastCommand");
    const int id_lastGroup = record.indexOf("lastGroup");
    while (query.next())
    {
        bool ok = false;
        const int chat_id = query.value(id_chat_id).toInt(&ok);
        if (ok) {
            ChatActions chatActions;
            chatActions.currentPlace    = (Content::Place)  query.value(id_currentPlace).toInt();
            chatActions.currentCommand  = (Content::Command)query.value(id_currentCommand).toInt();
            chatActions.lastPlace       = (Content::Place)  query.value(id_lastPlace).toInt();
            chatActions.lastCommand     = (Content::Command)query.value(id_lastCommand).toInt();
            chatActions.lastGroup       = query.value(id_lastGroup).toString();
            return chatActions;
        }
        else{
            qWarning() << "chatSettings is not valid:" << chat_id << Qt::endl;
        }
    }
    return ChatActions{};
}

bool DatabaseConnector::setChatSettings(const int64_t chat_id, const ChatSettings &chatSettings)
{
    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO user_chat_settings (language, quantityGroupsX, chat_id) "
                    "VALUES (:language, :quantityGroupsX, :chat_id)");
    query.bindValue(":language", chatSettings.language);
    query.bindValue(":quantityGroupsX", chatSettings.quantityGroupsX);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

ChatSettings DatabaseConnector::getChatSettings(const int64_t chat_id) const
{
    QSqlQuery query(QString("SELECT * FROM user_chat_settings WHERE (chat_id = %1)").arg(varinatChatId(chat_id).toString()));
    const QSqlRecord record = query.record();
    const int id_chat_id            = record.indexOf("chat_id");
    const int id_language           = record.indexOf("language");
    const int id_quantityGroupsX    = record.indexOf("quantityGroupsX");
    while (query.next())
    {
        bool ok_quantityGroupsX = false;
        int quantityGroupsX = query.value(id_quantityGroupsX).toInt(&ok_quantityGroupsX);
        if (!quantityGroupsX) {
            qWarning() << "Can not convert query to number(1)" << __FUNCTION__ << "chat_id" << chat_id << Qt::endl;
            quantityGroupsX = ChatSettings().quantityGroupsX;
        }
        ChatSettings chatSettings;
        chatSettings.language           = query.value(id_language).toString();
        chatSettings.quantityGroupsX    = quantityGroupsX;
        bool ok_chat_id = false;
        const int chat_id_read = query.value(id_chat_id).toInt(&ok_chat_id);
        if (!ok_chat_id) {
            qWarning() << "Can not convert query to number(2)" << __FUNCTION__ << "chat_id" << chat_id << Qt::endl;
            continue;
        }
        if (chat_id_read == chat_id) {
            return chatSettings;
        }
    }
    return ChatSettings{};
}

#ifdef QT_DEBUG
void DatabaseConnector::printDatabase(const int num) const
{
    qDebug() << "Begin" << __FUNCTION__ << num ;
    const auto allChatId = getAllChatId();
    for (const auto chat_id : allChatId) {
        qDebug() << getChat(chat_id);
    }
    qDebug() << "End" << __FUNCTION__ << num << Qt::endl;
}
#endif

QVector<std::int64_t> DatabaseConnector::getAllChatId() const
{
    // NOTE: maybe create new table all_chats
    QVector<std::int64_t> vec;
    QSqlQuery query;
    query.prepare("SELECT * FROM user_chat_actions");
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int id_chat_id    = record.indexOf("chat_id");
    while (query.next())
    {
        bool ok = false;
        const int chat_id = query.value(id_chat_id).toInt(&ok);
        if (!ok) {
            qWarning() << "Can not convert query to number" << __FUNCTION__ << "chat_id" << chat_id << Qt::endl;
            continue;
        }
        vec.append(chat_id);
    }
    return vec;
}

bool DatabaseConnector::addOnlyGroup(const QString &group_name, const GroupPosition &groupPosition, const int64_t chat_id)
{
    if (group_name.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO user_groups (group_name, groupPositionX, groupPositionY, chat_id) VALUES (:group_name, :groupPositionX, :groupPositionY, :chat_id)");
    query.bindValue(":group_name", group_name);
    query.bindValue(":groupPositionX", groupPosition.x);
    query.bindValue(":groupPositionY", groupPosition.y);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::addOnlyGroup(const std::string &group_name, const GroupPosition &groupPosition, const int64_t chat_id)
{
    return addOnlyGroup(QString::fromStdString(group_name), groupPosition, chat_id);
}

bool DatabaseConnector::deleteOnlyGroup(const QString &group_name, const int64_t chat_id)
{
    if (group_name.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM user_groups WHERE (group_name = :group_name AND chat_id = :chat_id)");
    query.bindValue(":group_name", group_name);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::deleteAllNotes(const QString &group_name, const int64_t chat_id)
{
    if (group_name.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM user_notes WHERE (group_name = :group_name AND chat_id = :chat_id)");
    query.bindValue(":group_name", group_name);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::existsNote(const QString &note_text, const QString &group_name, const int64_t chat_id) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM user_notes WHERE (note_text = ':note_text' AND group_name = :group_name AND chat_id = :chat_id);");
    query.bindValue(":note_text", note_text);
    query.bindValue(":group_name", group_name);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if (query.exec()){
        return query.next();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return query.next();
}

void DatabaseConnector::openDb(const QString &pathDatabase)
{
#ifdef QT_DEBUG
//        QFile::remove(pathDatabase);
#endif
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathDatabase);
    if(!db.open()){
        qWarning() << "Database not open" << pathDatabase << Qt::endl;
    }
}

void DatabaseConnector::createAllTables()
{
    createTable_UserGroups();
    createTable_UserNotes();
    createTable_UserChatActions();
    createTable_UserChatSettings();
}

bool DatabaseConnector::createTable_UserGroups()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS user_groups ("
            "group_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
            "group_name TEXT NOT NULL,"
            "groupPositionX INTEGER NOT NULL,"
            "groupPositionY INTEGER NOT NULL,"
            "chat_id INTEGER NOT NULL,"
            "FOREIGN KEY (chat_id) REFERENCES all_chats(chat_id) ON DELETE CASCADE"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table:" << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::createTable_UserNotes()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS user_notes ("
            "note_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
            "note_text TEXT NOT NULL,"
            "group_name TEXT NOT NULL,"
            "chat_id INTEGER NOT NULL,"
            "FOREIGN KEY (chat_id) REFERENCES all_chats(chat_id) ON DELETE CASCADE"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table:" << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::createTable_UserChatActions()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS user_chat_actions ("
            "chat_id INTEGER NOT NULL,"
            "currentPlace INTEGER DEFAULT 0,"
            "currentCommand INTEGER DEFAULT 0,"
            "lastPlace INTEGER DEFAULT 0,"
            "lastCommand INTEGER DEFAULT 0,"
            "lastGroup TEXT,"
            "PRIMARY KEY (chat_id),"
            "UNIQUE (chat_id)"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table:" << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::createTable_UserChatSettings()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS user_chat_settings ("
            "chat_id INTEGER NOT NULL,"
            "language TEXT NOT NULL,"
            "quantityGroupsX INTEGER NOT NULL,"
            "PRIMARY KEY (chat_id),"
            "UNIQUE (chat_id)"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table:" << query.lastError() << Qt::endl;
    return false;
}
