/**************************************************************************
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/.
**
**************************************************************************/

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
