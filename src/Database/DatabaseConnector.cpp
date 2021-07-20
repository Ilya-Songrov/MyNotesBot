#include "DatabaseConnector.h"

DatabaseConnector::DatabaseConnector(const QString &pathDatabase, QObject *parent) : QObject(parent)
{
#ifdef QT_DEBUG
    //    QFile::remove(pathDatabase);
#endif
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathDatabase);
    if(!db.open()){
        qWarning() << "Database not open" << pathDatabase << Qt::endl;
    }
    createDatabase();
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

bool DatabaseConnector::addNote(const QString &note, const QString &group, const int64_t chat_id)
{
    if (note.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    printDatabase(1);
    if (replaceNullOrExistsNote(note, group, chat_id) == 1) {
        return true;
    }
    printDatabase(2);
    QSqlQuery query;
    query.prepare("INSERT INTO my_notes (note, group, chat_id) VALUES (:note, :group, :chat_id)");
    query.bindValue(":note", note);
    query.bindValue(":group", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        printDatabase(3);
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    qFatal("Fatal");
    return false;
}

bool DatabaseConnector::addNote(const std::string &note, const QString &group, const int64_t chat_id)
{
    return addNote(QString::fromStdString(note), group, chat_id);
}

bool DatabaseConnector::addGroup(const QString &group, const int64_t chat_id)
{
    if (group.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    if (existsGroup(group, chat_id)) {
        return true;
    }
    QSqlQuery query;
    query.prepare("INSERT INTO my_notes (group, chat_id) VALUES (:group, :chat_id)");
    query.bindValue(":group", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::addGroup(const std::string &group, const int64_t chat_id)
{
    return addGroup(QString::fromStdString(group), chat_id);
}

bool DatabaseConnector::replaceNote(const QString &newNote, const QString &oldNote, const QString &group, const int64_t chat_id)
{
    if (newNote.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("UPDATE my_notes SET note = :oldNote WHERE note = :newNote AND group = :group AND chat_id = :chat_id");
    query.bindValue(":oldNote", oldNote);
    query.bindValue(":newNote", newNote);
    query.bindValue(":group", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::existsGroup(const QString &group, const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE chat_id = :chat_id AND group = :group");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":group", group);
    if (query.exec()){
        return query.next();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::existsGroup(const std::string &group, const int64_t chat_id)
{
    return existsGroup(QString::fromStdString(group), chat_id);
}

bool DatabaseConnector::deleteAllNotes(const QString &group, const int64_t chat_id)
{
    if (group.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be < 0" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM my_notes WHERE (group = :group AND chat_id = :chat_id)");
    query.bindValue(":group", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::setChatActions(const int64_t chat_id, const ChatActions &chatActions)
{
    QSqlQuery query;
    query.prepare("INSERT INTO all_chat_actions (chat_id, currentPlace, currentCommand, lastPlace, lastCommand, lastGroup) "
                    "VALUES (:chat_id, :currentPlace, :currentCommand, :lastPlace, :lastCommand, :lastGroup) "
                    "ON DUPLICATE KEY UPDATE (currentPlace, currentCommand, lastPlace, lastCommand, lastGroup) "
                    "(:currentPlace, :currentCommand, :lastPlace, :lastCommand, :lastGroup)");
    query.bindValue(":chat_id"          , varinatChatId(chat_id));
    query.bindValue(":currentPlace"     , chatActions.currentPlace);
    query.bindValue(":currentCommand"   , chatActions.currentCommand);
    query.bindValue(":lastPlace"        , chatActions.lastPlace);
    query.bindValue(":lastCommand"      , chatActions.lastCommand);
    query.bindValue(":lastGroup"        , chatActions.lastGroup);
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

ChatActions DatabaseConnector::getChatActions(const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM all_chat_actions WHERE chat_id = :chat_id");
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
    ChatActions chatActions;
    while (query.next())
    {
        bool ok = false;
        const int chat_id = query.value(id_chat_id).toInt(&ok);
        if (ok) {
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
    return {};
}

std::shared_ptr<QMap<uint64_t, ChatSettings> > DatabaseConnector::getAllChatSettings()
{
    std::shared_ptr<QMap<uint64_t, ChatSettings> > mapChatSettings(new QMap<uint64_t, ChatSettings>);
    fillMapChatSettings(mapChatSettings);
    return mapChatSettings;
}

ChatSettings DatabaseConnector::getChatSettings(const int64_t chat_id)
{
    std::shared_ptr<QMap<uint64_t, ChatSettings> > mapChatSettings(new QMap<uint64_t, ChatSettings>);
    fillMapChatSettings(mapChatSettings, chat_id);
    if (mapChatSettings->size() > 0) {
        return mapChatSettings->first();
    }
    return {};
}

bool DatabaseConnector::updateChatSettings(const int64_t chat_id, const ChatSettings &chatSettings)
{
    QSqlQuery query;
    query.prepare("UPDATE all_chat_settings SET (language = :language) WHERE (chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":language", chatSettings.language);
    if(query.exec()){
        return query.numRowsAffected();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

void DatabaseConnector::printDatabase(const int num) const
{
    qDebug() << "Begin" << num << __FUNCTION__;
    QSqlQuery query("SELECT * FROM my_notes");
    const QSqlRecord record = query.record();
    const int idChatId = record.indexOf("chat_id");
    const int idNote = record.indexOf("note");
    const int idGroup = record.indexOf("group");
    while (query.next())
    {
        const int chat_id = query.value(idChatId).toInt();
        const QString note = query.value(idNote).toString();
        const QString group = query.value(idGroup).toString();
        qDebug() << QString("chat_id (%1) group (%2) note (%3)").arg(chat_id).arg(group, note);
    }
    qDebug() << "End" << num << __FUNCTION__ << Qt::endl;
}

Chat DatabaseConnector::getChat(const int64_t chat_id)
{
    // TODO: maybe write me
}

QVector<Note> DatabaseConnector::getVecNotes(const QString &group, const int64_t chat_id)
{
    QVector<Note> vec;
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE chat_id = :chat_id AND group = :group");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":group", group);
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int idNote = record.indexOf("note");
    while (query.next())
    {
        const QString strNote = query.value(idNote).toString();
        if (strNote.isEmpty()) {
            continue;
        }
        Note note;
        note.note               = strNote;
        vec.append(note);
    }
    return vec;
}

QVector<Group> DatabaseConnector::getVecGroups(const int64_t chat_id)
{
    QVector<Group> vec;
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE chat_id = :chat_id");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int idGroup = record.indexOf("group");
    const int idGroupPositionX = record.indexOf("groupPositionX");
    const int idGroupPositionY = record.indexOf("groupPositionY");
    while (query.next())
    {
        const QString strGroup = query.value(idGroup).toString();
        if (strGroup.isEmpty()) {
            continue;
        }
        bool isX, isY;
        Group group;
        group.group             = strGroup;
        group.groupPositionX    = query.value(idGroupPositionX).toInt(&isX);
        group.groupPositionY    = query.value(idGroupPositionY).toInt(&isY);
        group.vecNotes          = getVecNotes(strGroup, chat_id);
        vec.append(group);
        if (!isX || !isY) {
            qWarning() << "Can not convert query to int" << Qt::endl;
        }
    }
    return vec;
}

bool DatabaseConnector::deleteNotes(const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM my_notes WHERE (chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::existsNote(const QString &note, const QString &group, const int64_t chat_id) const
{
    qDebug() << "note" << note << "group" << group << Qt::endl;
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE (note = ':note' AND group = :group AND chat_id = :chat_id);");
    query.bindValue(":note", note);
    query.bindValue(":group", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    qDebug() << "Text" << query.boundValues()<< Qt::endl;
    if (query.exec()){
        return query.next();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return query.next();
}

int DatabaseConnector::replaceNullOrExistsNote(const QString &newNote, const QString &group, const int64_t chat_id)
{
    if (newNote.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("UPDATE my_notes SET note = :newNote WHERE (note IS NULL OR note = :newNote) AND (chat_id = :chat_id AND group = :group)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":newNote", newNote);
    query.bindValue(":group", group);
    if(query.exec()){
        return query.numRowsAffected();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

void DatabaseConnector::fillMapChatSettings(std::shared_ptr<QMap<uint64_t, ChatSettings> > mapChatSettings, const int64_t chat_id)
{
    QString chatIfValid;
    if (chat_id != -1) {
        chatIfValid = QString(" WHERE (chat_id = %1)").arg(varinatChatId(chat_id).toString());
    }
    QSqlQuery query("SELECT * FROM all_chat_settings" + chatIfValid);
    const QSqlRecord record = query.record();
    const int id_chat_id = record.indexOf("chat_id");
    const int id_language = record.indexOf("language");
    while (query.next())
    {
        ChatSettings chatSettings;
        chatSettings.language = query.value(id_language).toString();
        bool ok = false;
        const int chat_id = query.value(id_chat_id).toInt(&ok);
        if (ok) {
            mapChatSettings->insert(chat_id, chatSettings);
        }
        else{
            qWarning() << "chatSettings is not valid:" << chat_id << Qt::endl;
        }
    }
}

void DatabaseConnector::createDatabase()
{
    createTable_Notes();
    createTable_AllChatActions();
    createTable_AllChatSettings();
}

bool DatabaseConnector::createTable_Notes()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS my_notes ("
            "note_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
            "note TEXT DEFAULT NULL,"
            "group TEXT NOT NULL,"
            "chat_id INTEGER NOT NULL,"
            "FOREIGN KEY (chat_id) REFERENCES all_chats(chat_id) ON DELETE CASCADE"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table 'my_notes':" << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::createTable_AllChatActions()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS all_chat_actions ("
            "chat_id INTEGER NOT NULL,"
            "currentPlace INTEGER DEFAULT 0,"
            "currentCommand INTEGER DEFAULT 0,"
            "lastPlace INTEGER DEFAULT 0,"
            "lastCommand INTEGER DEFAULT 0,"
            "lastGroup TEXT NOT NULL,"
            "PRIMARY KEY (chat_id),"
            "UNIQUE (chat_id)"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table 'all_chat_actions':" << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::createTable_AllChatSettings()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS all_chat_settings ("
            "chat_id INTEGER NOT NULL,"
            "language TEXT,"
            "PRIMARY KEY (chat_id),"
            "UNIQUE (chat_id)"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table 'all_chat_settings':" << query.lastError() << Qt::endl;
    return false;
}
