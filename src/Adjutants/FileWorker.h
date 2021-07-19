#pragma once

#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QFileInfo>
#include <QDir>

class FileWorker
{
public:
    static bool readFileJson(QJsonDocument *doc, const QString &path);
    static QJsonDocument readFileJson(const QString &path, bool *ok = nullptr);
};
