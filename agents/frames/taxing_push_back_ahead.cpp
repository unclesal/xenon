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
    _next_action.action = ACF_DOES_NOTHING;
    _next_action.priority = 20;
    _activated = false;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Обновление состояния фрейма                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void TaxingPushBackAhead::update( CmdAircraftCondition * cmd ) {
    
    _next_action.action = ACF_DOES_NOTHING;
    _activated = false;
    
    _environment->agents_mutex.lock();
    
    auto our_location = _ptr_acf->get_location();
    auto our_heading = _ptr_acf->vcl_condition.rotation.heading;
    
    for ( auto agent: _environment->agents ) {
        if ( agent.is_aircraft() && agent.ahead_me( our_location, our_heading ) ) {
            if ( agent.vcl_condition.current_action == ACF_DOES_PUSH_BACK ) {
                auto distance = xenon::distance2d( our_location, agent.vcl_condition.location );
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

