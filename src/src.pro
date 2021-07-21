QT      -= gui
QT      += sql

CONFIG += c++14 console
CONFIG -= app_bundle
TARGET = NotesBot

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../Exceptions/exceptions/exceptionerror.cpp \
    ../Exceptions/exceptions/unixsignalnotifier.cpp \
    Adjutants/AppTranslator.cpp \
    Adjutants/FileWorker.cpp \
    BotPlaces/PlaceAbstract.cpp \
    BotPlaces/PlaceAdditional/PlaceAdditional.cpp \
    BotPlaces/PlaceNotes/PlaceNotes.cpp \
    BotPlaces/PlaceStart/placestart.cpp \
    Content/content.cpp \
    Database/databaseconnector.cpp \
    Database/managerdb.cpp \
    GlobalData/GlobalData.cpp \
    ManagerBot.cpp \
    main.cpp \

HEADERS += \
    ../Exceptions/exceptions/applicationnotify.h \
    ../Exceptions/exceptions/exceptionerror.h \
    ../Exceptions/exceptions/unixsignalnotifier.h \
    ../builds_submodules/LogSaver/LogSaver/logsaver.h \
    Adjutants/AppTranslator.h \
    Adjutants/FileWorker.h \
    BotPlaces/PlaceAbstract.h \
    BotPlaces/PlaceAdditional/PlaceAdditional.h \
    BotPlaces/PlaceNotes/PlaceNotes.h \
    BotPlaces/PlaceStart/placestart.h \
    Content/chat.h \
    Content/chatactions.h \
    Content/chatsettings.h \
    Content/content.h \
    Database/databaseconnector.h \
    Database/managerdb.h \
    GlobalData/GlobalData.h \
    ManagerBot.h \
    tgbot/include/tgbot/tgbot.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += \
    ../Exceptions/exceptions \
    ../tgbot-cpp/include \
    ../builds_submodules/LogSaver/LogSaver \
    Adjutants \
    Database \
    Content \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources/translationFiles/translationFiles.qrc

TRANSLATIONS += \
    Resources/translationFiles/NotesBot_UA.ts \
LogSaver

### tgbot ###
unix:!macx: LIBS += -L$$PWD/../builds_submodules/tgbot -lTgBot
INCLUDEPATH += $$PWD/../builds_submodules/tgbot
DEPENDPATH  += $$PWD/../builds_submodules/tgbot
unix:!macx: PRE_TARGETDEPS += $$PWD/../builds_submodules/tgbot/libTgBot.a

#-lTgBot -lboost_system -lssl -lcrypto -lpthread
unix:!macx: LIBS += -lboost_system
unix:!macx: LIBS += -lssl
unix:!macx: LIBS += -lcrypto
unix:!macx: LIBS += -lpthread
### tgbot ###

### LogSaver ###
unix:!macx: LIBS += -L$$PWD/../builds_submodules/LogSaver/LogSaver -lLogSaver
INCLUDEPATH += $$PWD/../builds_submodules/LogSaver/LogSaver
DEPENDPATH  += $$PWD/../builds_submodules/LogSaver/LogSaver
unix:!macx: PRE_TARGETDEPS += $$PWD/../builds_submodules/LogSaver/LogSaver/libLogSaver.a
### LogSaver ###


### cqtdeployer ###
CONFIG(release, debug | release) {
QMAKE_POST_LINK=/bin/bash -c \"cqtdeployer -bin $${OUT_PWD}/$${TARGET} -targetDir DeployKit_$${TARGET} -qmake $${QMAKE_QMAKE} -libDir $${PWD} \
                               -recursiveDepth 10 -qmlDir $${PWD} force-clear ; \"
}
### cqtdeployer ###
