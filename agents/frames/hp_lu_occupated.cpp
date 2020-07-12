// *********************************************************************************************************************
// *                        Фрейм ожидания на предварительном старте, если исполнительный занят                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 jul 2020 at 12:21 *
// *********************************************************************************************************************
#include "hp_lu_occupated.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

HpLuOccupated::HpLuOccupated( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment)
    : StateFrame( bimbo, environment )
{
    _next_action.action = ACF_DOES_NOTHING;
    _next_action.priority = 20;
    _activated = false;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Обновление состояния фрейма                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void xenon::HpLuOccupated::update(xenon::CmdAircraftCondition* cmd) {
    
    _next_action.action = ACF_DOES_NOTHING;
    _activated = false;
    
    _environment->agents_mutex.lock();
    
    auto our_location = _ptr_acf->get_location();
    auto our_heading = _ptr_acf->vcl_condition.rotation.heading;
    
    for ( auto agent: _environment->agents ) {
        if ( agent.is_aircraft() && agent.ahead_me(our_location, our_heading) ) {
            auto distance = xenon::distance2d( our_location, agent.vcl_condition.location );
            
            if ( distance <= 150.0 ) {
                
//                 Logger::log(
//                     "HP_LU_Occupated. " + _ptr_acf->vcl_condition.agent_name + " -> " + agent.vcl_condition.agent_name + ", dist=" + std::to_string( distance )
//                 );
                
                _next_action.action = ACF_DOES_WAITING_TAKE_OFF_APPROVAL;
                _activated = true;
                break;
            };
        }
    }
    _environment->agents_mutex.unlock();
}


