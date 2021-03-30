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

bool ManagerDatabase::addNote(const QString &note, const int64_t chat_id)
{
    if (note.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    if (existsPrayerNeed(note, chat_id)) {
        return true;
    }
    if (!existsChatId(chat_id)) {
        inserNewChat(chat_id);
    }
    QSqlQuery query;
    query.prepare("INSERT INTO my_notes (note, chat_id) VALUES (:note, :chat_id)");
    query.bindValue(":note", note);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::addNote(const std::string &note, const int64_t chat_id)
{
    return addNote(QString::fromStdString(note), chat_id);
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

bool ManagerDatabase::deleteAllMyNotes(const int64_t chat_id)
{
    const bool retChat = deleteChat(chat_id);
    const bool retNeed = deleteNote(chat_id);
    return retChat && retNeed;
}

QStringList ManagerDatabase::getListMyNotes(const int64_t chat_id, const ManagerDatabase::TypeListMyNotes typeList)
{
    const auto vecNeeds = getVecMyNotes(chat_id, typeList);
    QStringList list;
    for (const auto &prayerNeed: vecNeeds) {
        list.append(prayerNeed.need);
    }
    return list;
}

QVector<ManagerDatabase::Note> ManagerDatabase::getVecMyNotes(const int64_t chat_id, const ManagerDatabase::TypeListMyNotes typeList)
{
    QVector<Note> vec;
    QSqlQuery query;
    if (typeList == TypeListMyNotes::PrayerNeedsWithoutAnswerOfGod) {
        query.prepare("SELECT * FROM my_notes WHERE chat_id = :chat_id AND answer IS NULL");
    }
    else if (typeList == TypeListMyNotes::PrayerNeedsWithAnswerOfGod) {
        query.prepare("SELECT * FROM my_notes WHERE (chat_id = :chat_id AND answer IS NOT NULL)");
    }
    else if (typeList == TypeListMyNotes::PrayerNeedsAll) {
        query.prepare("SELECT * FROM my_notes WHERE (chat_id = :chat_id)");
    }
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(!query.exec()){
        qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
        return {};
    }
    const QSqlRecord record = query.record();
    const int idNeedId = record.indexOf("need_id");
    const int idNeed = record.indexOf("need");
    const int idAnswer = record.indexOf("answer");
    while (query.next())
    {
        Note prayerNeed;
        prayerNeed.need_id  = query.value(idNeedId).toString();
        prayerNeed.note     = "◾️ " + query.value(idNeed).toString();
        prayerNeed.answer   = query.value(idAnswer).toString();
        vec.append(prayerNeed);
    }
    return vec;
}

void ManagerDatabase::printDatabase() const
{
    qDebug() << "Begin" << __FUNCTION__;
    QSqlQuery query("SELECT * FROM my_notes");
    const QSqlRecord record = query.record();
    const int idNeedId = record.indexOf("need_id");
    const int idNeed = record.indexOf("need");
    const int idAnswer = record.indexOf("answer");
    const int idChat_id = record.indexOf("chat_id");
    while (query.next())
    {
        const QString need_id = query.value(idNeedId).toString();
        const QString need = query.value(idNeed).toString();
        const QString answer = query.value(idAnswer).toString();
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

bool ManagerDatabase::existsPrayerNeed(const QString &prayerNeed, const int64_t chat_id) const
{
    QSqlQuery query;
    query.prepare("SELECT * FROM my_notes WHERE (need = ':prayerNeed' AND chat_id = :chat_id);");
    query.bindValue(":prayerNeed", prayerNeed);
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
