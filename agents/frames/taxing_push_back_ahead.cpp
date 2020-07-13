// *********************************************************************************************************************
// *                          Если при рулении прямо по курсу выполняется выталкивание - ждем.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 jul 2020 at 19:35 *
// *********************************************************************************************************************
#include "taxing_push_back_ahead.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

TaxingPushBackAhead::TaxingPushBackAhead( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment )
    : StateFrame( bimbo, environment )
{    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Обновление состояния фрейма                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void TaxingPushBackAhead::update() {
    
    _before_update();
    _environment->agents_mutex.lock();
        
    for ( auto agent: _environment->agents ) {
        auto delta = xenon::course_to( _our_location, _our_rotation.heading, agent.vcl_condition.location );
        bool ahead_me = (( delta >= 270 ) || ( delta <= 90 ));
        if ( agent.is_aircraft() && ahead_me ) {
            if ( agent.vcl_condition.current_action == ACF_DOES_PUSH_BACK ) {
                auto distance = xenon::distance2d( _our_location, agent.vcl_condition.location );
                if ( distance <= MIN_PUSH_BACK_AHEAD_ME ) {
                    _next_action.action = ACF_DOES_TAXING_STOP;
                    _activated = true;
                    break;
                }
            };
        }
            
    };
    
    _environment->agents_mutex.unlock();
}

