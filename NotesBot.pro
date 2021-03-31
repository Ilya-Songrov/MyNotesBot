QT      -= gui
QT      += sql

CONFIG += c++14 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Adjutants/AppTranslator.cpp \
    Adjutants/FileWorker.cpp \
    Anomalies/Errors.cpp \
    Anomalies/ExceptionHandler.cpp \
    BotPlaces/PlaceAbstract.cpp \
    BotPlaces/PlaceAdditional/PlaceAdditional.cpp \
    BotPlaces/PlaceNotes/PlaceNotes.cpp \
    BotPlaces/PlaceStart/placestart.cpp \
    Content/ChatInfo.cpp \
    Content/Content.cpp \
    Database/ManagerDatabase.cpp \
    GlobalData/GlobalData.cpp \
    ManagerBot.cpp \
    main.cpp \

HEADERS += \
    Adjutants/AppTranslator.h \
    Adjutants/FileWorker.h \
    Anomalies/ApplicationNotify.h \
    Anomalies/Errors.h \
    Anomalies/ExceptionHandler.h \
    BotPlaces/PlaceAbstract.h \
    BotPlaces/PlaceAdditional/PlaceAdditional.h \
    BotPlaces/PlaceNotes/PlaceNotes.h \
    BotPlaces/PlaceStart/placestart.h \
    Content/ChatInfo.h \
    Content/Content.h \
    Database/ManagerDatabase.h \
    GlobalData/GlobalData.h \
    ManagerBot.h \
    tgbot/include/tgbot/tgbot.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += \
    tgbot/include \
    Adjutants \
    Anomalies \
    Database \


unix:!macx: LIBS += -L$$PWD/tgbot -lTgBot
INCLUDEPATH += $$PWD/tgbot
DEPENDPATH  += $$PWD/tgbot
unix:!macx: PRE_TARGETDEPS += $$PWD/tgbot/libTgBot.a

#-lTgBot -lboost_system -lssl -lcrypto -lpthread
unix:!macx: LIBS += -lboost_system
unix:!macx: LIBS += -lssl
unix:!macx: LIBS += -lcrypto
unix:!macx: LIBS += -lpthread

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources/translationFiles/translationFiles.qrc

TRANSLATIONS += \
    Resources/translationFiles/appTranslator_ru_UA.ts \


### ApplicationLogLib ###
HEADERS += \
    ApplicationLogLib/ApplicationLogLib.h \
    ApplicationLogLib/ApplicationLogLib_global.h \

unix:!macx: LIBS += -L$$PWD/ApplicationLogLib/ -lApplicationLogLib
INCLUDEPATH += $$PWD/ApplicationLogLib
DEPENDPATH  += $$PWD/ApplicationLogLib
unix:!macx: PRE_TARGETDEPS += $$PWD/ApplicationLogLib/libApplicationLogLib.so
#QMAKE_LFLAGS += -Wl,-rpath,"'\$$ORIGIN'"   # OR
QMAKE_RPATHDIR += ":'\$$ORIGIN'"            # OR
### ApplicationLogLib ###

