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
#include "chat.h"
#include "chatactions.h"

class DatabaseConnector : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseConnector(const QString &pathDatabase, QObject *parent = nullptr);
    virtual ~DatabaseConnector();

    bool isOpen() const;

    bool addNote(const QString &note_text, const QString &group_name, const std::int64_t chat_id);
    bool addNote(const std::string &note_text, const QString &group_name, const std::int64_t chat_id);

    bool replaceNote(const QString &newNote, const QString &oldNote, const QString &group_name, const std::int64_t chat_id);

    bool deleteNoteViaID(const int note_id, const std::int64_t chat_id);

    bool existsGroup(const QString &group_name, const std::int64_t chat_id);
    bool existsGroup(const std::string &group_name, const std::int64_t chat_id);

    Chat getChat(const std::int64_t chat_id) const;
    QVector<Group> getVecGroups(const std::int64_t chat_id) const;
    QVector<Note> getVecNotes(const QString &group_name, const std::int64_t chat_id) const;

    bool setChatActions(const ChatActions &chatActions, const std::int64_t chat_id);
    ChatActions getChatActions(const std::int64_t chat_id) const;

    bool setChatSettings(const std::int64_t chat_id, const ChatSettings &chatSettings);
    ChatSettings getChatSettings(const std::int64_t chat_id) const;

#ifdef QT_DEBUG
    void printDatabase(const int num) const;
#endif
protected:
    QVector<std::int64_t> getAllChatId() const;

    bool addOnlyGroup(const QString &group_name, const GroupPosition &groupPosition, const std::int64_t chat_id);
    bool addOnlyGroup(const std::string &group_name, const GroupPosition &groupPosition, const std::int64_t chat_id);

    bool deleteOnlyGroup(const QString &group_name, const std::int64_t chat_id);
    bool deleteAllNotes(const QString &group_name, const std::int64_t chat_id);

    bool existsNote(const QString &note_text, const QString &group_name, const std::int64_t chat_id) const;
    inline QVariant varinatChatId(const std::int64_t chat_id) const { return QVariant::fromValue(chat_id); }

private:
    void openDb(const QString &pathDatabase);
    void createAllTables();
    bool createTable_UserGroups();
    bool createTable_UserNotes();
    bool createTable_UserChatActions();
    bool createTable_UserChatSettings();

private:
    QSqlDatabase db;
};

