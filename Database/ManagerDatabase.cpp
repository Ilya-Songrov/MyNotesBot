#include "ManagerDatabase.h"

ManagerDatabase::ManagerDatabase(const QString &pathDatabase, QObject *parent) : QObject(parent)
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
}

ManagerDatabase::~ManagerDatabase()
{
    if (db.isOpen()){
        db.close();
    }
}

bool ManagerDatabase::isOpen() const
{
    return db.isOpen();
}

bool ManagerDatabase::addNote(const QString &note, const QString &group, const int64_t chat_id)
{
    if (note.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    if (replaceNullOrExistsNote(note, group, chat_id) == 1) {
        return true;
    }
    if (!existsChatId(chat_id)) {
        inserNewChat(chat_id);
    }
    QSqlQuery query;
    query.prepare("INSERT INTO my_notes (note, group_note, chat_id) VALUES (:note, :group_note, :chat_id)");
    query.bindValue(":note", note);
    query.bindValue(":group_note", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::addNote(const std::string &note, const QString &group, const int64_t chat_id)
{
    return addNote(QString::fromStdString(note), group, chat_id);
}

bool ManagerDatabase::addGroup(const QString &group, const int64_t chat_id)
{
    if (group.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    if (existsGroup(group, chat_id)) {
        return true;
    }
    if (!existsChatId(chat_id)) {
        inserNewChat(chat_id);
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

bool ManagerDatabase::addGroup(const std::string &group, const int64_t chat_id)
{
    return addGroup(QString::fromStdString(group), chat_id);
}

bool ManagerDatabase::replaceNote(const QString &newNote, const QString &oldNote, const QString &group, const int64_t chat_id)
{
    if (newNote.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("UPDATE my_notes SET note = :oldNote WHERE note = :newNote AND group_note = :group_note AND chat_id = :chat_id ");
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

bool ManagerDatabase::existsGroup(const QString &group, const int64_t chat_id)
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

bool ManagerDatabase::existsGroup(const std::string &group, const int64_t chat_id)
{
    return existsGroup(QString::fromStdString(group), chat_id);
}

bool ManagerDatabase::deleteAllNotes(const int note_id, const int64_t chat_id)
{
    if (note_id < 0){
        qWarning() << __FUNCTION__ << "failed: value cannot be -1" << Qt::endl;
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

bool ManagerDatabase::deleteAllNotes(const QString &group, const int64_t chat_id)
{
    if (group.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be -1" << Qt::endl;
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

QStringList ManagerDatabase::getListNotes(const std::string &group, const int64_t chat_id)
{
    QStringList list;
    const auto vecNotess = getVecNotes(QString::fromStdString(group), chat_id);
    for (const auto &note: vecNotess) {
        list.append(note.note);
    }
    return list;
}

QVector<ManagerDatabase::OneNote> ManagerDatabase::getVecNotes(const QString &group, const int64_t chat_id)
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
        OneNote prayerNeed;
        prayerNeed.note_id  = query.value(idNoteId).toString();
        prayerNeed.note     = "◾️ " + query.value(idNote).toString();
        prayerNeed.group    = query.value(idGroup).toString();
        vec.append(prayerNeed);
    }
    return vec;
}

QStringList ManagerDatabase::getListGroups(const int64_t chat_id)
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

void ManagerDatabase::printDatabase() const
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

bool ManagerDatabase::inserNewChat(const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("INSERT INTO all_chats (chat_id) VALUES (:chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::deleteChat(const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM all_chats WHERE (chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::deleteNotes(const int64_t chat_id)
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

bool ManagerDatabase::existsChatId(const int64_t chat_id) const
{
    QSqlQuery query;
    query.prepare("SELECT chat_id FROM all_chats WHERE (chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if (query.exec()){
        return query.next();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::existsNote(const QString &note, const QString &group, const int64_t chat_id) const
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

int ManagerDatabase::replaceNullOrExistsNote(const QString &newNote, const QString &group, const int64_t chat_id)
{
    if (newNote.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("UPDATE my_notes SET note = :newNote WHERE (note IS NULL OR note = :newNote) AND (group_note = :group_note AND chat_id = :chat_id)");
    query.bindValue(":newNote", newNote);
    query.bindValue(":group_note", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return query.numRowsAffected();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

void ManagerDatabase::createDatabase()
{
    createTable_AllChats();
    createTable_Notes();
}

bool ManagerDatabase::createTable_AllChats()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS all_chats ("
            "chat_id INTEGER NOT NULL,"
            "PRIMARY KEY (chat_id),"
            "UNIQUE (chat_id)"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table 'all_chats':" << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::createTable_Notes()
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
