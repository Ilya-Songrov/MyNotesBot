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

#include "GlobalData.h"

std::unique_ptr<Bot> bot(nullptr);
std::unique_ptr<ManagerDb> managerDb(nullptr);

void initGlobalData(const QString &token)
{
    bot.reset(new Bot(token.toStdString()));
    managerDb.reset(new ManagerDb(QString("%1Database.db").arg(QCoreApplication::applicationName())));
}
