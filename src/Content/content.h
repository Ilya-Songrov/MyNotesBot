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

#pragma once

#include <QObject>
#include <QDebug>

class Content : public QObject
{
    Q_OBJECT
public:
    explicit Content(QObject *parent = nullptr);
    enum class ContentType{
        Message     = 0,
        Command,
        UnknownCommand,
        NonCommandMessage,
        InlineQuery,
        ChosenInlineResult,
        CallbackQuery,
    };
    enum Place{
        Start       = 0,
        Notes,
        Additional,
        MultiPlace,
    };
    Q_ENUM(Place)
    enum Command{
        // TODO: maybe delete NoCommand
        NoCommand   = 0,

        Start_ChooseLanguage,

        MultiPlace_Start,
        MultiPlace_Help,
        MultiPlace_AnyMessage,
        MultiPlace_AnyCallbackQuery = MultiPlace_AnyMessage,

        Notes_AddNote,
        Notes_RemoveNote,
        Notes_ListNotesOfGroup,
        Notes_CreateGroup,
        Notes_RemoveGroup,
        Notes_GroupWasSelectedToAdd,
        Notes_GroupWasSelectedToRemove,

        Additional_Additional,
        Additional_ShowHistory,
        Additional_DeleteHistory,
        Additional_Developer,
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

