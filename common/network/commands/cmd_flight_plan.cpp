// *********************************************************************************************************************
// *                                          Команда передачи по сети полетного плана                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 09 jul 2020 at 09:34 *
// *********************************************************************************************************************
#include "cmd_flight_plan.h"
#include "utils.hpp"

using namespace xenon;
using namespace std;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Пустой конструктор                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdFlightPlan::CmdFlightPlan()
    : AbstractCommand()
{
    _command_name = "CmdFlightPlan";
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Конструктор от параметров                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdFlightPlan::CmdFlightPlan( const vehicle_condition_t & vcl_condition, const xenon::FlightPlan & flight_plan)
    : AbstractCommand( SAY_TO_ENVIRONMENT, vcl_condition )
{
    _command_name = "CmdFlightPlan";
    __fp = flight_plan;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Преобразование из объекта в JSON                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void CmdFlightPlan::to_json( JSON & json) {
    
    AbstractCommand::to_json( json );
    json["flight_number"] = __fp.__flight_number;
    json["departure"] = __fp.__departure;
    json["destination"] = __fp.__destination;
    json["alternate"] = __fp.__alternate;
    json["cruise_altitude"] = __fp.__cruise_altitude;
    
    auto way = JSON::array();
    for ( auto wp : __fp.__way ) {
        JSON element;
        element["name"] = wp.name;
        element["type"] = (uint16_t) wp.type;
        element["latitude"] = wp.location.latitude;
        element["longitude"] = wp.location.longitude;
        element["altitude"] = wp.location.altitude;
        element["speed"] = wp.speed;        
        element["action_to_achieve"] = (uint16_t) wp.action_to_achieve;
        way.push_back( element );
    };
    json["way"] = way;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из JSONа в объект                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void CmdFlightPlan::from_json( JSON & json ) {
    
    AbstractCommand::from_json( json );
    __fp.__flight_number = json.value("flight_number", "");
    __fp.__departure = json.value("departure", "");
    __fp.__destination = json.value("destination", "");
    __fp.__alternate = json.value("alternate", vector<string>());
    __fp.__cruise_altitude = json.value("cruise_altitude", 0.0);
    
    __fp.__way.clear();
    auto way = json.value("way", JSON::array());
    for ( auto element: way ) {
        waypoint_t wp;
        wp.name = element.value("name", "");
        wp.type = ( waypoint_type_t ) element.value( "type", 0 );
        wp.location.latitude = element.value( "latitude", 0.0 );
        wp.location.longitude = element.value( "longitude", 0.0 );
        wp.location.altitude = element.value( "altitude", 0.0 );
        wp.speed = element.value( "speed", 0.0 );        
        wp.action_to_achieve = (xenon::aircraft_action_t) element.value( "action_to_achieve", 0 );
        __fp.__way.push_back( wp );
    };
    
    // Дистанции и курсы - не передаются по сети, они считаются на приемной стороне.
    __fp.recalculate();
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Выполнение команды на стороне сервера                                           *
// *                                                                                                                   *
// *********************************************************************************************************************
#ifdef SERVER_SIDE
void CmdFlightPlan::execute_on_server( ConnectedClientCore * client, ClientsListener * server ) {
    server->send_to_those_who_can_hear( this );
};
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Выполнение команды на стороне агента                                           *
// *                                                                                                                   *
// *********************************************************************************************************************
#ifdef INSIDE_AGENT
void CmdFlightPlan::execute_on_agent( ConnectedCommunicatorReactor * current_agent ){
    
    // Эта часть гарантированно добавит агента, если его еще не было.
    AbstractCommand::execute_on_agent( current_agent );
    
    std::string uuid = agent_uuid();
    
    current_agent->agents_mutex.lock();
    
    for ( auto agent: current_agent->agents ) {
        if ( agent.agent_uuid() == uuid ) {
            if ( ! agent.flight_plan ) agent.flight_plan = new FlightPlan( __fp );
            else * agent.flight_plan = __fp;
            break;
        };
    };
    
    current_agent->agents_mutex.unlock();
};
#endif
