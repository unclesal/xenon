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
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Метод пересчета состояния фрейма после получения команды по сети                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void PushBackAllowed::update() {
    
    _before_update();
    
    _environment->agents_mutex.lock();
    
    for ( auto agent : _environment->agents ) {
        
        // Нас интересуют только те агенты, которые самолеты. И эта 
        // штука должна двигаться. То есть быть в push-back или таксинг        
        
//         Logger::log(
//             _ptr_acf->vcl_condition.agent_name + " -> " + agent.vcl_condition.agent_name + ", " + state_to_string( agent.vcl_condition.current_state )
//             + ", " + action_to_string( agent.vcl_condition.current_action ) + ", dis=" + to_string( distance ) +  ", is_on_taxiway=" + to_string( agent.is_on_taxiway() )
//             + ", is_aircraft=" + to_string( agent.is_aircraft() )
//         );
        
        if ( agent.is_aircraft() && agent.is_on_taxiway() ) {
            // Вот теперь все. Меряем до нее расстояние.
        
            auto distance = xenon::distance2d( _ptr_acf->get_location(), agent.vcl_condition.location);
            if ( distance <= MIN_ALLOWABLE_PUSH_BACK_DISTANCE ) {
                
                // Logger::log( _ptr_acf->vcl_condition.agent_name + " waiting push back activated.");
                
                _next_action.action = ACF_DOES_WAITING_PUSH_BACK;
                _activated = true;
                break;
            }
        }        
    }
    
    _environment->agents_mutex.unlock();
    
}


