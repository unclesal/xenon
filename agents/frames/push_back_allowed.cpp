// *********************************************************************************************************************
// *                       Запрет выталкивания, если рядом уже есть самолет, который двигается                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 07 jul 2020 at 13:25 *
// *********************************************************************************************************************
#include "push_back_allowed.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

PushBackAllowed::PushBackAllowed(
    BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment
)
    : StateFrame( bimbo, environment )
{
    _action = bimbo->front_waypoint().action_to_achieve;
    _likeliness = 20;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Метод пересчета состояния фрейма после получения команды по сети                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void PushBackAllowed::update( CmdAircraftCondition * cmd ) {
    
    _action = _ptr_acf->front_waypoint().action_to_achieve;
    _environment->agents_mutex.lock();
    
    for ( auto agent : _environment->agents ) {
        // Нас интересуют только те агенты, которые самолеты.        
        if ( 
            agent.vcl_condition.agent_type == AGENT_AIRCRAFT
            || agent.vcl_condition.agent_type == AGENT_XPLANE
        ) {            
            // И эта штука должна двигаться. То есть быть в push-back или таксинг
            if ( 
                agent.vcl_condition.current_action == ACF_DOES_PUSH_BACK
                || agent.vcl_condition.current_action == ACF_DOES_SLOW_TAXING
                || agent.vcl_condition.current_action == ACF_DOES_NORMAL_TAXING
            ) {
                // Вот теперь все. Меряем до нее расстояние.
                auto distance = xenon::distance2d( _ptr_acf->get_location(), agent.vcl_condition.location);                
                if ( distance <= 100.0 ) {
                    _action = ACF_DOES_NOTHING;
                    break;
                }
            }
        }
    }
    _environment->agents_mutex.unlock();
    
}


