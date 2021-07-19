#include "FileWorker.h"


bool FileWorker::readFileJson(QJsonDocument *doc, const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        const QString error = QString("Error QIODevice::ReadOnly | QIODevice::Text (%1) (%2)").arg(file.errorString(), file.fileName());
        qWarning() << error << Qt::endl;
        return false;
    }
    const QByteArray arr = file.readAll();
    QJsonParseError errorJson;
    *doc = QJsonDocument::fromJson(arr, &errorJson);
    if (errorJson.error != QJsonParseError::NoError) {
        qWarning() << "QJsonParseError: " + errorJson.errorString() << " arr: " + arr << " file: " + file.fileName() << Qt::endl;
        return false;
    }
    file.close();
    return true;
}

QJsonDocument FileWorker::readFileJson(const QString &path, bool *ok)
{
    QJsonDocument doc;
    const bool ret = readFileJson(&doc, path);
    if (ok) {
        *ok = ret;
    }
    return doc;
}
