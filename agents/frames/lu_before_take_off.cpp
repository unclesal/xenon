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
    _next_action.action = ACF_DOES_NOTHING;
    _next_action.priority = 20;
    _activated = false;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Обновление состояния фрейма                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void LUBeforeTakeOff::update( CmdAircraftCondition * cmd ) {
    _next_action.action = ACF_DOES_NOTHING;
    _activated = false;
    
    _environment->agents_mutex.lock();
    
    auto our_location = _ptr_acf->get_location();
    auto our_heading = _ptr_acf->vcl_condition.rotation.heading;
    
    for ( auto agent: _environment->agents ) {
        if ( agent.is_aircraft() ) {
            // Здесь он должен быть не просто "впереди", а точно по курсу.
            // Иначе захватывает соседей на коробочке.
            auto his_location = agent.vcl_condition.location;
            // Курс на него. Интересуют только те самолеты, которые впереди нас. Как 
            // бы не айс, если останавливаться при нарушении дистанции будет - передний.
                        
            auto bearing = xenon::bearing( our_location, his_location );
            
            // --------------------------------------------------
            // Это - правильная комбинация. Вместе с 
            // нормализацией дает именно тот курс, который надо.
            // --------------------------------------------------
             
            auto delta = bearing - our_heading;
            xenon::normalize_degrees( delta );
            
            // --------------------------------------------------
            
            if (( delta >= 355 ) || ( delta <= 5 )) {
            
                if (
                    agent.vcl_condition.current_action == ACF_DOES_BECOMING
                    || agent.vcl_condition.current_state == ACF_STATE_AIRBORNED
                ) {
                    auto distance = xenon::distance2d(our_location, agent.vcl_condition.location);
                    if ( distance <= MIN_ALLOWABLE_TAKE_OFF_DISTANCE ) {
                        Logger::log(
                            "LuBeforeTakeOff: " + _ptr_acf->vcl_condition.agent_name + " -> " + agent.vcl_condition.agent_name + ", distance=" + std::to_string( distance )
                        );
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




