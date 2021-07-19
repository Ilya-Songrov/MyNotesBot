#include "DatabaseConnector.h"

DatabaseConnector::DatabaseConnector(const QString &pathDatabase, QObject *parent) : QObject(parent)
{
#ifdef QT_DEBUG
    //    QFile::remove(pathDatabase);
#endif
    const bool needCreateTables = !QFileInfo::exists(pathDatabase);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(pathDatabase);
    if (db.open() && needCreateTables) {
    }
    else if(!db.open()){
        qWarning() << "Database not open" << pathDatabase << Qt::endl;
    }
    createDatabase();
    addNote(QString("111"), "qqq", 395107597);
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
//    if (replaceNullOrExistsNote(note, group, chat_id) == 1) {
//        return true;
//    }
    printDatabase();
    QSqlQuery query;
//    query.prepare("UPDATE my_notes SET note = :note WHERE (chat_id = :chat_id AND note IS NULL AND group_note = :group_note) "
//                    "IF (@@ROWCOUNT = 0) PRINT 'my message'");
//    query.prepare("INSERT INTO my_notes (note, group_note, chat_id) VALUES (:note, :group_note, :chat_id)");
    query.prepare("INSERT INTO my_notes (note, group_note, chat_id) "
                    "SELECT (note, group_note, chat_id) "
                    "VALUES (:note, :group_note, :chat_id)");
    query.bindValue(":note", note);
    query.bindValue(":group_note", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        printDatabase();
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
    query.prepare("INSERT INTO my_notes (group_note, chat_id) VALUES (:group_note, :chat_id)");
    query.bindValue(":group_note", group);
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
    query.prepare("UPDATE my_notes SET note = :oldNote WHERE note = :newNote AND group_note = :group_note AND chat_id = :chat_id");
    query.bindValue(":oldNote", oldNote);
    query.bindValue(":newNote", newNote);
    query.bindValue(":group_note", group);
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
    query.prepare("SELECT * FROM my_notes WHERE chat_id = :chat_id AND group_note = :group_note");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":group_note", group);
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

bool DatabaseConnector::deleteAllNotes(const int note_id, const int64_t chat_id)
{
    if (note_id < 0){
        qWarning() << __FUNCTION__ << "failed: value cannot be < 0" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM my_notes WHERE (note_id = :note_id AND chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":note_id", note_id);
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool DatabaseConnector::deleteAllNotes(const QString &group, const int64_t chat_id)
{
    if (group.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be < 0" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM my_notes WHERE (group_note = :group_note AND chat_id = :chat_id)");
    query.bindValue(":group_note", group);
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

QStringList DatabaseConnector::getListNotes(const std::string &group, const int64_t chat_id)
{
    QStringList list;
    const auto vecNotess = getVecNotes(QString::fromStdString(group), chat_id);
    for (const auto &note: vecNotess) {
        list.append(note.note);
    }
    return list;
}

QVector<DatabaseConnector::OneNote> DatabaseConnector::getVecNotes(const QString &group, const int64_t chat_id)
{
    QVector<OneNote> vec;
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE chat_id = :chat_id AND group_note = :group_note");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":group_note", group);
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int idNoteId = record.indexOf("note_id");
    const int idNote = record.indexOf("note");
    const int idGroup = record.indexOf("group_note");
    while (query.next())
    {
        const QString strNote = query.value(idNote).toString();
        if (strNote.isEmpty()) {
            continue;
        }
        OneNote note;
        note.note     = "◾️ " + strNote;
        note.group    = query.value(idGroup).toString();
        note.note_id  = query.value(idNoteId).toString();
        vec.append(note);
    }
    return vec;
}

QStringList DatabaseConnector::getListGroups(const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE (chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    QStringList list;
    const QSqlRecord record = query.record();
    const int idGroup = record.indexOf("group_note");
    while (query.next())
    {
        const auto group = query.value(idGroup).toString();
        if (!group.isEmpty() && !list.contains(group)) {
            list.append(group);
        }
    }
    return list;
}

void DatabaseConnector::printDatabase() const
{
    qDebug() << "Begin" << __FUNCTION__;
    QSqlQuery query("SELECT * FROM my_notes");
    const QSqlRecord record = query.record();
    const int idNoteId = record.indexOf("note_id");
    const int idNote = record.indexOf("note");
    const int idGroup = record.indexOf("group_note");
    const int idChat_id = record.indexOf("chat_id");
    while (query.next())
    {
        const int chat_id = query.value(idChat_id).toInt();
        const QString note_id = query.value(idNoteId).toString();
        const QString note = query.value(idNote).toString();
        const QString group = query.value(idGroup).toString();
        qDebug() << QString("chat_id (%1) note_id (%2) note (%3) group (%4)").arg(chat_id).arg(note_id, note, group);
    }
    qDebug() << "End" << __FUNCTION__ << Qt::endl;
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
    query.prepare("SELECT * FROM my_notes WHERE (note = ':note' AND group_note = :group_note AND chat_id = :chat_id);");
    query.bindValue(":note", note);
    query.bindValue(":group_note", group);
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
    query.prepare("UPDATE my_notes SET note = :newNote WHERE (note IS NULL OR note = :newNote) AND (chat_id = :chat_id AND group_note = :group_note)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":newNote", newNote);
    query.bindValue(":group_note", group);
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
            "group_note TEXT NOT NULL,"
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
