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
    _say_to = say_to;
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из объекта в JSON                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractCommand::to_json ( JSON & json ) {
    
    JSONAble::to_json( json );
    
    json["packet_number"] = __packet_number;
    json["say_to"] = (uint8_t) _say_to;
    
    json["agent_uuid"] = _vcl_condition.agent_uuid;
    if ( !_vcl_condition.to_agent_uuid.empty() ) json["to_agent_uuid"] = _vcl_condition.to_agent_uuid;
    json["agent_type"] = ( uint8_t ) _vcl_condition.agent_type;
    json["agent_name"] = _vcl_condition.agent_name;
    
    // Прижатость к земле.
    json[ "is_clamped_to_ground" ] = _vcl_condition.is_clamped_to_ground;
    
    json["latitude"] = _vcl_condition.location.latitude;
    json["longitude"] = _vcl_condition.location.longitude;
    json["altitude"] = _vcl_condition.location.altitude;
    
    json["current_state"] = (uint16_t) _vcl_condition.current_state;
    json["current_action"] = (uint16_t) _vcl_condition.current_action;

        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Преобразование из JSONа в объект                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractCommand::from_json ( JSON & json ) {
    
    JSONAble::from_json( json );
    
    __packet_number = json.value( "packet_number", 0 );
    _say_to = (say_to_t) json.value( "say_to", (uint8_t) SAY_TO_UNKNOWN );
    
    _vcl_condition.agent_uuid = json.value("agent_uuid", "");
    _vcl_condition.to_agent_uuid = json.value( "to_agent_uuid", "" );
    _vcl_condition.agent_type = ( agent_t ) json.value("agent_type", (uint8_t)AGENT_UNKNOWN );
    _vcl_condition.agent_name = json.value("agent_name", "UNKNOWN");
    
    // Прижатость к земле.
    _vcl_condition.is_clamped_to_ground = json.value( "is_clamped_to_ground", false );
    
    _vcl_condition.location.latitude = json.value( "latitude", 0.0 );
    _vcl_condition.location.longitude = json.value( "longitude", 0.0 );
    _vcl_condition.location.altitude = json.value( "altitude", 0.0 );
    
    _vcl_condition.current_state = (aircraft_state_t) json.value( "current_state", 0 );
    _vcl_condition.current_action = ( aircraft_action_t ) json.value( "current_action", 0 );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Выполнение на стороне сервера - по умолчанию                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef SERVER_SIDE
void AbstractCommand::execute_on_server( ConnectedClientCore * client, ClientsListener * server ) {    
    server->send_to_those_who_can_hear( this );
}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Выполнение команды на стороне агента - по умолчанию                                 *
// *                                                                                                                   *
// *********************************************************************************************************************
#ifdef INSIDE_AGENT
void AbstractCommand::execute_on_agent( ConnectedCommunicatorReactor * current_agent ) {
    
    current_agent->agents_mutex.lock();

    bool exists = false;
    std::string uuid = agent_uuid();
    // Ищем в коллекции уже существующий агент с таким же uuid'ом.
    for ( int i=0; i < current_agent->agents.size(); ++ i ) {
        if ( current_agent->agents.at( i ).agent_uuid() == uuid ) {
            current_agent->agents.at( i ).vcl_condition = _vcl_condition;            
            exists = true;
        }
    }

    if ( ! exists ) {
        ConnectedCommunicatorReactor::another_agent_t a;
        a.vcl_condition = _vcl_condition;
        a.acf_condition = nullptr;        
        current_agent->agents.push_back( a ); 
    }

    current_agent->agents_mutex.unlock();

}
#endif
