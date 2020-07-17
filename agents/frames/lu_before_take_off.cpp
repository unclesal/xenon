// *********************************************************************************************************************
// *                      Фрейм проверки на исполнительном старте на предмет ранее взлетевшего самолета                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 jul 2020 at 11:20 *
// *********************************************************************************************************************
#include "lu_before_take_off.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

LUBeforeTakeOff::LUBeforeTakeOff( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment) 
    : StateFrame( bimbo, environment )
{
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Обновление состояния фрейма                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void LUBeforeTakeOff::update() {
    
    _before_update();    
    
    // Если уже начал разбег - проверять бессмысленно, пусть взлетает.
    if ( _ptr_acf->graph->current_action_is ( ACF_DOES_TAKE_OFF ) ) return;
    
    _environment->agents_mutex.lock();
        
    for ( auto agent: _environment->agents ) {
        if ( agent.is_aircraft() ) {
            // Здесь он должен быть не просто "впереди", а точно по курсу.
            // Иначе захватывает соседей на коробочке.
            auto his_location = agent.vcl_condition.location;
            auto delta = xenon::course_to( _our_location, _our_rotation.heading, his_location );
            
            if (( delta >= 355 ) || ( delta <= 5 )) {
            
                if (
                    agent.vcl_condition.current_action == ACF_DOES_TAKE_OFF
                    || agent.vcl_condition.current_action == ACF_DOES_BECOMING
                    || agent.vcl_condition.current_state == ACF_STATE_AIRBORNED
                ) {
                    auto distance = xenon::distance2d(_our_location, agent.vcl_condition.location);
                    if ( distance <= MIN_ALLOWABLE_TAKE_OFF_DISTANCE ) {
                        
//                         Logger::log(
//                             "LuBeforeTakeOff: " + _ptr_acf->vcl_condition.agent_name + " -> " + agent.vcl_condition.agent_name + ", distance=" + std::to_string( distance )
//                         );
                        
                        _next_action.action = ACF_DOES_WAITING_TAKE_OFF_APPROVAL;
                        _activated = true;
                        break;
                    };
                };
                
            };
        };
    };
    
    _environment->agents_mutex.unlock();
}




