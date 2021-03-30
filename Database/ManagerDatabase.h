#pragma once

#include <QObject>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QFile>
#include <QFileInfo>
#include <QVariant>

class ManagerDatabase : public QObject
{
    Q_OBJECT

public:
    struct MyNote{
        QString note;
        QString group;
        QString note_id;
    };

    explicit ManagerDatabase(const QString &pathDatabase, QObject *parent = nullptr);
    ~ManagerDatabase();

    bool isOpen() const;

    bool addPrayerNeed(const QString &prayerNeed, const std::int64_t chat_id);
    bool addPrayerNeed(const std::string &prayerNeed, const std::int64_t chat_id);

    bool addAnswerOfGod(const QString &answer, const int need_id);
    bool addAnswerOfGod(const std::string &answer, const int need_id);

    bool deletePrayerNeed(const int need_id, const std::int64_t chat_id);

    bool deleteAllPrayerNeeds(const std::int64_t chat_id);

    QStringList getListMyNotes(const std::int64_t chat_id, const std::string group);
    QVector<MyNote> getVecPrayerNeeds(const std::int64_t chat_id, const std::string group);
    void printDatabase() const;
private:
    bool inserNewChat(const std::int64_t chat_id);
    bool deleteChat(const std::int64_t chat_id);
    bool deletePrayerNeed(const std::int64_t chat_id);
    bool existsChatId(const std::int64_t chat_id) const;
    bool existsPrayerNeed(const QString &prayerNeed, const std::int64_t chat_id) const;

    inline QVariant varinatChatId(const std::int64_t chat_id) const { return QVariant::fromValue(chat_id); }

    void createDatabase();
    bool createTable_AllChats();
    bool createTable_PrayerNeeds();
private:
    QSqlDatabase db;
};

