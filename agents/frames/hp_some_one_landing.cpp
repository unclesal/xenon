// *********************************************************************************************************************
// *                             Предварительный старт: если кто-то в стадии посадки, то ждем.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 12 jul 2020 at 14:50 *
// *********************************************************************************************************************

#include "hp_some_one_landing.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

HpSomeOneLanding::HpSomeOneLanding( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment)
    : StateFrame( bimbo, environment )
{    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Обновление состояния фрейма                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void HpSomeOneLanding::update() {
    
    _before_update();
    
    if ( _ptr_acf->vcl_condition.current_action == ACF_DOES_LINING_UP ) {
        // Опоздали. Если он уже пошел выравниваться - то смысла нет останавливаться.
        return;
    }
    
    _environment->agents_mutex.lock();
    
    for ( auto agent: _environment->agents ) {
        if ( agent.is_aircraft() ) {
            if ( 
                agent.vcl_condition.current_state == ACF_STATE_ON_FINAL
                || agent.vcl_condition.current_action == ACF_DOES_LANDING
            ) {
                _next_action.action = ACF_DOES_WAITING_TAKE_OFF_APPROVAL;
                _activated = true;
                break;
            }
        };
    };
    
    _environment->agents_mutex.unlock();
}



