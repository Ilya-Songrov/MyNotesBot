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

#include "chatsettings.h"
#include "ChatActions.h"

class DatabaseConnector : public QObject
{
    Q_OBJECT

public:
    struct OneNote{
        QString note;
        QString group;
        QString note_id;
    };

    explicit DatabaseConnector(const QString &pathDatabase, QObject *parent = nullptr);
    virtual ~DatabaseConnector();

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

    bool setChatActions(const std::int64_t chat_id, const ChatActions &chatActions);
    ChatActions getChatActions(const std::int64_t chat_id);

    std::shared_ptr<QMap<std::uint64_t, ChatSettings> > getAllChatSettings();
    virtual ChatSettings getChatSettings(const std::int64_t chat_id);
    virtual bool updateChatSettings(const std::int64_t chat_id, const ChatSettings &chatSettings);

    QStringList getListNotes(const std::string &group, const std::int64_t chat_id);
    QVector<OneNote> getVecNotes(const QString &group, const std::int64_t chat_id);
    QStringList getListGroups(const std::int64_t chat_id);

    void printDatabase() const;
private:
    bool deleteNotes(const std::int64_t chat_id);
    bool existsNote(const QString &note, const QString &group, const std::int64_t chat_id) const;
    int replaceNullOrExistsNote(const QString &newNote, const QString &group, const std::int64_t chat_id);

    inline QVariant varinatChatId(const std::int64_t chat_id) const { return QVariant::fromValue(chat_id); }

    void fillMapChatSettings(std::shared_ptr<QMap<uint64_t, ChatSettings> > mapChatSettings, const std::int64_t chat_id = -1);

    void createDatabase();
    bool createTable_Notes();
    bool createTable_AllChatActions();
    bool createTable_AllChatSettings();
private:
    QSqlDatabase db;
};

