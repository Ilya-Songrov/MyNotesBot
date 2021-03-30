#pragma once

#include <QObject>
#include <QDebug>

class Content : public QObject
{
    Q_OBJECT
public:
    explicit Content(QObject *parent = nullptr);
    enum class ContentType{
        Message,
        Command,
        UnknownCommand,
        NonCommandMessage,
        InlineQuery,
        ChosenInlineResult,
        CallbackQuery,
    };
    enum class Place{
        ThyCloset,
        Additional,
        Church,
        MultiPlace,
    };
    Q_ENUM(Place)
    enum Command{
        NoCommand,

        ThyCloset_AddPrayerNeed,
        ThyCloset_AddAnswerOfGod,
        ThyCloset_WriteAnswerOfGod,
        ThyCloset_ListPrayerNeed,

        Additional_Additional,
        Additional_ShowHistory,
        Additional_DeletePrayerNeed,
        Additional_DeleteHistory,
        Additional_Developer,

        MultiPlace_Start,
        MultiPlace_Help,
        MultiPlace_AnyMessage,
        MultiPlace_AnyCallbackQuery = MultiPlace_AnyMessage,
    };
    Q_ENUM(Command)
    struct PlaceCommand{
        Place place     = Place::MultiPlace;
        Command command = MultiPlace_AnyMessage;
        PlaceCommand(const Place pl, const Command cm) : place(pl), command(cm) {}
        PlaceCommand(){}
    };

    static void initContent();
    static PlaceCommand getPlaceCommand(const QString &command);
    static PlaceCommand getPlaceCommand(const std::string &command);
    static QString getCommandStr(const Command command);

private:
    static QVector<QPair<PlaceCommand, QString> > vecPlaceCommand;
};

