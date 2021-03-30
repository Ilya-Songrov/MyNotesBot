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
    enum TypeListPrayerNeeds{
        PrayerNeedsWithoutAnswerOfGod,
        PrayerNeedsWithAnswerOfGod,
        PrayerNeedsAll,
    };
    struct PrayerNeed{
        QString need;
        QString answer;
        QString need_id;
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

    QStringList getListPrayerNeeds(const std::int64_t chat_id, const TypeListPrayerNeeds typeList = TypeListPrayerNeeds::PrayerNeedsWithoutAnswerOfGod);
    QVector<PrayerNeed> getVecPrayerNeeds(const std::int64_t chat_id, const TypeListPrayerNeeds typeList = TypeListPrayerNeeds::PrayerNeedsWithoutAnswerOfGod);
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

