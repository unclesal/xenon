// *********************************************************************************************************************
// *                      В агенте (т.е. в "основной сущности") была достигнута точка полетного плана.                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 jul 2020 at 11:42 *
// *********************************************************************************************************************
#include "cmd_waypoint_reached.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Пустой конструктор                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdWaypointReached::CmdWaypointReached() 
    : AbstractCommand() 
{
    _command_name = "CmdWaypointReached";
    __npp = 0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Конструктор от параметров                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdWaypointReached::CmdWaypointReached(const vehicle_condition_t & vcl_condition, const uint16_t & npp) 
    : AbstractCommand( SAY_TO_ENVIRONMENT, vcl_condition )
{
    _command_name = "CmdWaypointReached";
    __npp = npp;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из объекта в JSON                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void CmdWaypointReached::to_json( JSON & json) {
    AbstractCommand::to_json( json );
    json["npp"] = __npp;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Преобразование из JSONа в объект                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void CmdWaypointReached::from_json( JSON & json ) {
    AbstractCommand::from_json( json );
    __npp = json.value("npp", 0 );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Выполнение команды на стороне сервера                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef SERVER_SIDE
void CmdWaypointReached::execute_on_server( ConnectedClientCore * client, ClientsListener * server ) {
    
    AbstractCommand::execute_on_server( client, server );
    
    if ( client->flight_plan ) {
        client->flight_plan->erase_up_to( __npp );
    };
    
}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Выполнение команды на стороне (другого) агента                                  *
// *                                                                                                                   *
// *********************************************************************************************************************
#ifdef INSIDE_AGENT
void CmdWaypointReached::execute_on_agent( ConnectedCommunicatorReactor * current_agent ) {
    
    AbstractCommand::execute_on_agent( current_agent );
    
    current_agent->agents_mutex.lock();
    
    for ( auto agent: current_agent->agents ) {
        if ( agent.agent_uuid() == agent_uuid() ) {
            if ( agent.flight_plan ) {
                agent.flight_plan->erase_up_to( __npp );
            }
            break;
        };
    };
    
    current_agent->agents_mutex.unlock();
};
#endif
