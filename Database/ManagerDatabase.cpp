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

bool ManagerDatabase::addNote(const QString &note, const QString group, const int64_t chat_id)
{
    if (note.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    if (existsNote(note, group, chat_id)) {
        return true;
    }
    if (!existsChatId(chat_id)) {
        inserNewChat(chat_id);
    }
    QSqlQuery query;
    query.prepare("INSERT INTO my_notes (note, group, chat_id) VALUES (:note, :group, :chat_id)");
    query.bindValue(":note", note);
    query.bindValue(":group", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::addNote(const std::string &note, const std::string &group, const int64_t chat_id)
{
    return addNote(QString::fromStdString(note), QString::fromStdString(group), chat_id);
}

bool ManagerDatabase::addAnswerOfGod(const QString &answer, const int need_id)
{
    if (answer.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("UPDATE my_notes SET answer = :answer WHERE need_id = :need_id");
    query.bindValue(":answer", answer);
    query.bindValue(":need_id", need_id);
    if(query.exec()){
        printDatabase();
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::addAnswerOfGod(const std::string &answer, const int need_id)
{
    return addAnswerOfGod(QString::fromStdString(answer), need_id);
}

bool ManagerDatabase::deleteNote(const int note_id, const int64_t chat_id)
{
    if (note_id == -1){
        qWarning() << __FUNCTION__ << "failed: value cannot be -1" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM my_notes WHERE (need_id = :need_id AND chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":need_id", note_id);
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::deleteAllNotes(const int64_t chat_id)
{
    const bool retChat = deleteChat(chat_id);
    const bool retNeed = deleteNote(chat_id);
    return retChat && retNeed;
}

QStringList ManagerDatabase::getListNotes(const int64_t chat_id, const std::string group)
{
    QStringList list;
    const auto vecNotess = getVecNotes(chat_id, group);
    for (const auto &note: vecNotess) {
        list.append(note.note);
    }
    return list;
}

QStringList ManagerDatabase::getListGroups(const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("SELECT group FROM my_notes WHERE (chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    QStringList list;
    const QSqlRecord record = query.record();
    const int idGroup = record.indexOf("group");
    while (query.next())
    {
        const auto group = query.value(idGroup).toString();
        if (!group.isEmpty()) {
            list.append(group);
        }
    }
    return list;
}

QVector<ManagerDatabase::OneNote> ManagerDatabase::getVecNotes(const int64_t chat_id, const std::string group)
{
    QVector<OneNote> vec;
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE chat_id = :chat_id AND group = :group");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":group", QString::fromStdString(group));
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int idNoteId = record.indexOf("note_id");
    const int idNote = record.indexOf("note");
    const int idGroup = record.indexOf("group");
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

void ManagerDatabase::printDatabase() const
{
    qDebug() << "Begin" << __FUNCTION__;
    QSqlQuery query("SELECT * FROM my_notes");
    const QSqlRecord record = query.record();
    const int idNoteId = record.indexOf("note_id");
    const int idNote = record.indexOf("note");
    const int idGroup = record.indexOf("group");
    const int idChat_id = record.indexOf("chat_id");
    while (query.next())
    {
        const QString need_id = query.value(idNoteId).toString();
        const QString need = query.value(idNote).toString();
        const QString answer = query.value(idGroup).toString();
        const int chat_id = query.value(idChat_id).toInt();
        qDebug() << QString("chat_id (%1) need_id (%2) need (%3) answer (%4)").arg(chat_id).arg(need_id, need, answer);
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

bool ManagerDatabase::deleteNote(const int64_t chat_id)
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
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE (note = ':note' AND group = :group AND chat_id = :chat_id);");
    query.bindValue(":note", note);
    query.bindValue(":group", group);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if (query.exec()){
        return query.next();
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return query.next();
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
            "note TEXT NOT NULL,"
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
