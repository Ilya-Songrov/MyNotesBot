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
    struct OneNote{
        QString note;
        QString group;
        QString note_id;
    };

    explicit ManagerDatabase(const QString &pathDatabase, QObject *parent = nullptr);
    ~ManagerDatabase();

    bool isOpen() const;

    bool addNote(const QString &note, const QString &group, const std::int64_t chat_id);
    bool addNote(const std::string &note, const QString &group, const std::int64_t chat_id);

    bool addGroup(const QString &group, const std::int64_t chat_id);
    bool addGroup(const std::string &group, const std::int64_t chat_id);

    bool replaceNote(const QString &newNote, const QString &oldNote, const QString &group, const std::int64_t chat_id);

    bool existsGroup(const QString &group, const std::int64_t chat_id);
    bool existsGroup(const std::string &group, const std::int64_t chat_id);

    bool deleteAllNotes(const int note_id, const std::int64_t chat_id);
    bool deleteAllNotes(const QString &group, const std::int64_t chat_id);

    QStringList getListNotes(const std::string &group, const std::int64_t chat_id);
    QVector<OneNote> getVecNotes(const QString &group, const std::int64_t chat_id);
    QStringList getListGroups(const std::int64_t chat_id);

    void printDatabase() const;
private:
    bool inserNewChat(const std::int64_t chat_id);
    bool deleteChat(const std::int64_t chat_id);
    bool deleteNotes(const std::int64_t chat_id);
    bool existsChatId(const std::int64_t chat_id) const;
    bool existsNote(const QString &note, const QString &group, const std::int64_t chat_id) const;
    int replaceNullOrExistsNote(const QString &newNote, const QString &group, const std::int64_t chat_id);

    inline QVariant varinatChatId(const std::int64_t chat_id) const { return QVariant::fromValue(chat_id); }

    void createDatabase();
    bool createTable_AllChats();
    bool createTable_Notes();
private:
    QSqlDatabase db;
};

