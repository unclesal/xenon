// *********************************************************************************************************************
// *                                             Команда установки чего-нибудь.                                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 sep 2019 at 14:54 *
// *********************************************************************************************************************
#include "CommandSet.h"
using namespace xenon;
using namespace std;

string CommandSet::COMMAND_NAME = "CommandSet";
string CommandSet::SECTION_AUTOPILOT = "AUTOPILOT";
string CommandSet::SECTION_USER_AIRCRAFT = "USER_AIRCRAFT";

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор.                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

CommandSet::CommandSet(SET_COMMANDS_T set_command, int ivalue, float fvalue )
    : JSONAble()
{
    _command_name = CommandSet::COMMAND_NAME;
    
    _section = "UNKNOWN";

    _set_command = set_command;
    __set_section();

    _i_value = ivalue;
    _f_value = fvalue;
    // _b_value = bvalue;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Установка секции в зависимости от имени команды                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void CommandSet::__set_section() {
    
    switch ( _set_command ) {

        case SET_AUTOPILOT_MODE:
        case SET_AUTOPILOT_STATE_FLAGS:
        case SET_AUTOPILOT_HEADING:
        case SET_AUTOPILOT_ALTITUDE:
        case SET_AUTOPILOT_AIR_SPEED:
        case SET_AUTOPILOT_VERTICAL_VELOCITY:
        case SET_AUTOTHROTTLE:
            _section = SECTION_AUTOPILOT;
        break;

        case SET_HSI_SELECTOR:
            _section = SECTION_USER_AIRCRAFT;
        break;

#ifdef EXTERNAL_SETTER
        default:
            qDebug() << "CommandSet::__set_section, unhandled name " << static_cast<int>( _set_command );
#endif

    }    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Преобразование из объекта в JSON                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void CommandSet::to_json( JSON & json) {
    
    JSONAble::to_json( json );

    json["set_command"] = static_cast<int>( _set_command );
    json["i_value"] = _i_value;
    json["f_value"] = _f_value;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Преобразование из JSONа в объект                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void CommandSet::from_json( JSON & json ) {

    JSONAble::from_json( json );

    int i_command = json.value( "set_command", -1 );
    _set_command = static_cast<SET_COMMANDS_T>( i_command );
    __set_section();

    _i_value = json.value( "i_value", 0 );
    _f_value = static_cast<float>( json.value( "f_value", 0.0 ) );
    
};
