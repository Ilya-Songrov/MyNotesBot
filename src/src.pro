QT      -= gui
QT      += sql

CONFIG += c++14 console
CONFIG -= app_bundle

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
    Content/Content.cpp \
    Database/DatabaseConnector.cpp \
    Database/managerdatabase.cpp \
    GlobalData/GlobalData.cpp \
    ManagerBot.cpp \
    main.cpp \

HEADERS += \
    ../Exceptions/exceptions/applicationnotify.h \
    ../Exceptions/exceptions/exceptionerror.h \
    ../Exceptions/exceptions/unixsignalnotifier.h \
    ../LogSaver/build/LogSaver/logsaver.h \
    Adjutants/AppTranslator.h \
    Adjutants/FileWorker.h \
    BotPlaces/PlaceAbstract.h \
    BotPlaces/PlaceAdditional/PlaceAdditional.h \
    BotPlaces/PlaceNotes/PlaceNotes.h \
    BotPlaces/PlaceStart/placestart.h \
    Content/ChatActions.h \
    Content/Content.h \
    Content/chatsettings.h \
    Database/DatabaseConnector.h \
    Database/managerdatabase.h \
    GlobalData/GlobalData.h \
    ManagerBot.h \
    tgbot/include/tgbot/tgbot.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += \
    ../tgbot-cpp/include \
    ../Exceptions/exceptions \
    ../LogSaver/build/LogSaver \
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


### tgbot ###
unix:!macx: LIBS += -L$$PWD/../tgbot-cpp -lTgBot
INCLUDEPATH += $$PWD/../tgbot-cpp
DEPENDPATH  += $$PWD/../tgbot-cpp
unix:!macx: PRE_TARGETDEPS += $$PWD/../tgbot-cpp/libTgBot.a

#-lTgBot -lboost_system -lssl -lcrypto -lpthread
unix:!macx: LIBS += -lboost_system
unix:!macx: LIBS += -lssl
unix:!macx: LIBS += -lcrypto
unix:!macx: LIBS += -lpthread
### tgbot ###

### LogSaver ###
unix:!macx: LIBS += -L$$PWD/../LogSaver/build/LogSaver -lLogSaver
INCLUDEPATH += $$PWD/../LogSaver/build/LogSaver
DEPENDPATH  += $$PWD/../LogSaver/build/LogSaver
unix:!macx: PRE_TARGETDEPS += $$PWD/../LogSaver/build/LogSaver/libLogSaver.a
### LogSaver ###


### cqtdeployer ###
CONFIG(release, debug | release) {
QMAKE_POST_LINK=/bin/bash -c \"cqtdeployer -bin $${OUT_PWD}/$${TARGET} -targetDir DeployKit_$${TARGET} -qmake $${QMAKE_QMAKE} -libDir $${PWD} \
                               -recursiveDepth 10 -qmlDir $${PWD} force-clear ; \"
}
### cqtdeployer ###
