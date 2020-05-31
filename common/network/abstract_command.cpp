// *********************************************************************************************************************
// *                                    Общий предок всех передаваемых по сети команд (v2.0)                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 10:10 *
// *********************************************************************************************************************

#include "abstract_command.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractCommand::AbstractCommand() : JSONAble() {
    __packet_number = 0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Конструктор от параметров                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractCommand::AbstractCommand( const say_to_t & say_to, const vehicle_condition_t & vcl_condition ) 
    : JSONAble()
{
    __packet_number = 0;
    _vcl_condition = vcl_condition;
    _vcl_condition.say_to = say_to;
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из объекта в JSON                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractCommand::to_json ( JSON & json ) {
    JSONAble::to_json( json );
    json["packet_number"] = __packet_number;
    json["say_to"] = (uint8_t) _vcl_condition.say_to;
    
    json["agent_uuid"] = _vcl_condition.agent_uuid;
    if ( !_vcl_condition.to_agent_uuid.empty() ) json["to_agent_uuid"] = _vcl_condition.to_agent_uuid;
    json["agent_type"] = ( uint8_t ) _vcl_condition.agent_type;
    json["agent_name"] = _vcl_condition.agent_name;
    
    json["latitude"] = _vcl_condition.location.latitude;
    json["longitude"] = _vcl_condition.location.longitude;
    json["altitude"] = _vcl_condition.location.altitude;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Преобразование из JSONа в объект                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractCommand::from_json ( JSON & json ) {
    
    JSONAble::from_json( json );
    __packet_number = json.value( "packet_number", 0 );
    _say_to = (say_to_t) json.value( "say_to", (uint8_t)SAY_TO_UNKNOWN );
    
    _vcl_condition.agent_uuid = json.value("agent_uuid", "");
    _vcl_condition.to_agent_uuid = json.value( "to_agent_uuid", "" );
    _vcl_condition.agent_type = ( agent_t ) json.value("agent_type", (uint8_t)AGENT_UNKNOWN );
    _vcl_condition.agent_name = json.value("agent_name", "UNKNOWN");
    
    _vcl_condition.location.latitude = json.value( "latitude", 0.0 );
    _vcl_condition.location.longitude = json.value( "longitude", 0.0 );
    _vcl_condition.location.altitude = json.value( "altitude", 0.0 );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Выполнение на стороне сервера - по умолчанию                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef SERVER_SIDE
void AbstractCommand::execute_on_server( ConnectedClientListener * client, ClientsListener * server ) {    
    server->send_to_those_who_can_hear( this );
}
#endif
