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
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Обновление состояния фрейма                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void xenon::HpLuOccupated::update() {
    
    _before_update();
        
    _environment->agents_mutex.lock();
        
    for ( auto agent: _environment->agents ) {
        auto delta = xenon::course_to( _our_location, _our_rotation.heading, agent.vcl_condition.location );
        bool ahead_me = (( delta >= 270 ) || ( delta <= 90 ));

        if ( agent.is_aircraft() && ahead_me ) {
            
            auto distance = xenon::distance2d( _our_location, agent.vcl_condition.location );
            
            if ( distance <= MIN_HP_LU_OCCUPATED ) {
                
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


