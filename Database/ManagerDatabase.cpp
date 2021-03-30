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

bool ManagerDatabase::addPrayerNeed(const QString &prayerNeed, const int64_t chat_id)
{
    if (prayerNeed.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    if (existsPrayerNeed(prayerNeed, chat_id)) {
        return true;
    }
    if (!existsChatId(chat_id)) {
        inserNewChat(chat_id);
    }
    QSqlQuery query;
    query.prepare("INSERT INTO prayer_needs (need, chat_id) VALUES (:prayerNeed, :chat_id)");
    query.bindValue(":prayerNeed", prayerNeed);
    query.bindValue(":chat_id", varinatChatId(chat_id));
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::addPrayerNeed(const std::string &prayerNeed, const int64_t chat_id)
{
    return addPrayerNeed(QString::fromStdString(prayerNeed), chat_id);
}

bool ManagerDatabase::addAnswerOfGod(const QString &answer, const int need_id)
{
    if (answer.isEmpty()){
        qWarning() << __FUNCTION__ << "failed: value cannot be empty" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("UPDATE prayer_needs SET answer = :answer WHERE need_id = :need_id");
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

bool ManagerDatabase::deletePrayerNeed(const int need_id, const int64_t chat_id)
{
    if (need_id == -1){
        qWarning() << __FUNCTION__ << "failed: value cannot be -1" << Qt::endl;
        return false;
    }
    QSqlQuery query;
    query.prepare("DELETE FROM prayer_needs WHERE (need_id = :need_id AND chat_id = :chat_id)");
    query.bindValue(":chat_id", varinatChatId(chat_id));
    query.bindValue(":need_id", need_id);
    if(query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "failed: " << query.lastError() << Qt::endl;
    return false;
}

bool ManagerDatabase::deleteAllPrayerNeeds(const int64_t chat_id)
{
    const bool retChat = deleteChat(chat_id);
    const bool retNeed = deletePrayerNeed(chat_id);
    return retChat && retNeed;
}

QStringList ManagerDatabase::getListPrayerNeeds(const int64_t chat_id, const ManagerDatabase::TypeListPrayerNeeds typeList)
{
    const auto vecNeeds = getVecPrayerNeeds(chat_id, typeList);
    QStringList list;
    for (const auto &prayerNeed: vecNeeds) {
        list.append(prayerNeed.need);
    }
    return list;
}

QVector<ManagerDatabase::PrayerNeed> ManagerDatabase::getVecPrayerNeeds(const int64_t chat_id, const ManagerDatabase::TypeListPrayerNeeds typeList)
{
    QVector<PrayerNeed> vec;
    QSqlQuery query;
    if (typeList == TypeListPrayerNeeds::PrayerNeedsWithoutAnswerOfGod) {
        query.prepare("SELECT * FROM prayer_needs WHERE chat_id = :chat_id AND answer IS NULL");
    }
    else if (typeList == TypeListPrayerNeeds::PrayerNeedsWithAnswerOfGod) {
        query.prepare("SELECT * FROM prayer_needs WHERE (chat_id = :chat_id AND answer IS NOT NULL)");
    }
    else if (typeList == TypeListPrayerNeeds::PrayerNeedsAll) {
        query.prepare("SELECT * FROM prayer_needs WHERE (chat_id = :chat_id)");
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
        PrayerNeed prayerNeed;
        prayerNeed.need_id  = query.value(idNeedId).toString();
        prayerNeed.need     = "◾️ " + query.value(idNeed).toString();
        prayerNeed.answer   = query.value(idAnswer).toString();
        vec.append(prayerNeed);
    }
    return vec;
}

void ManagerDatabase::printDatabase() const
{
    qDebug() << "Begin" << __FUNCTION__;
    QSqlQuery query("SELECT * FROM prayer_needs");
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

bool ManagerDatabase::deletePrayerNeed(const int64_t chat_id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM prayer_needs WHERE (chat_id = :chat_id)");
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
    query.prepare("SELECT * FROM prayer_needs WHERE (need = ':prayerNeed' AND chat_id = :chat_id);");
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
    createTable_PrayerNeeds();
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

bool ManagerDatabase::createTable_PrayerNeeds()
{
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS prayer_needs ("
            "need_id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT,"
            "need TEXT NOT NULL,"
            "answer TEXT DEFAULT NULL,"
            "chat_id INTEGER NOT NULL,"
            "FOREIGN KEY (chat_id) REFERENCES all_chats(chat_id) ON DELETE CASCADE"
            ");");

    if (query.exec()){
        return true;
    }
    qWarning() << __FUNCTION__ << "Couldn't create the table 'prayer_needs':" << query.lastError() << Qt::endl;
    return false;
}
