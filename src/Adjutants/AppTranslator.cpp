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

#include "AppTranslator.h"


AppTranslator::AppTranslator(const QString &pathTranslationFile, QObject *parent) :
    QObject(parent)
{
    // 0.
    /*
        AppTranslator appTranslator("qrc:Resources/translationFiles/appTranslator_ru_UA.qm");
    */
    // 1. add to (.pro) file:
    /*    TRANSLATIONS += \
              Resources/translationFiles/appTranslator_ru_UA.ts \
              Resources/translationFiles/appTranslator_ru_RU.ts
    */
    //    OTHER_FILES += $${TRANSLATIONS}
    // 2. QtCreator -> Tools -> External -> Linguist -> Update Translations (lupdate)
    // 3. run: cd ~/Qt/5.15.2/gcc_64/bin/ ; ./linguist
    // 4. QtCreator -> Tools -> External -> Linguist -> Release Translations (lrelease)
    // 5. create $$PWD/Resources/translationFiles/translationFiles.qrc and add appTranslator_ru_UA.qm, appTranslator_ru_RU.qm (prefix: translationFiles)
    /*    RESOURCES += \
              translationFiles.qrc
    */


    // pathTranslationFile == :/translationFiles/translationFiles/appTranslator_ru_UA.qm
    loadTranslator(pathTranslationFile);
}

void AppTranslator::setNewLanguage(const QString &pathTranslationFile)
{
    loadTranslator(pathTranslationFile);
}

void AppTranslator::loadTranslator(const QString &file)
{
    static QTranslator translator;
    QCoreApplication::removeTranslator(&translator);
    translator.load(file);
    QCoreApplication::installTranslator(&translator);
    testTranslator();
}

void AppTranslator::testTranslator()
{
    qDebug() << "Translation for Hello world - " << QObject::tr("Test - Hello world") << Qt::endl;
}
