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
    _next_action.action = ACF_DOES_NOTHING;
    _next_action.priority = 20;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Метод пересчета состояния фрейма после получения команды по сети                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void PushBackAllowed::update( CmdAircraftCondition * cmd ) {
    
    _next_action.action = ACF_DOES_NOTHING;
    
    _environment->agents_mutex.lock();
    
    for ( auto agent : _environment->agents ) {
        // Нас интересуют только те агенты, которые самолеты. И эта 
        // штука должна двигаться. То есть быть в push-back или таксинг
        if ( agent.is_aircraft() && agent.is_on_taxiway() ) {
            // Вот теперь все. Меряем до нее расстояние.
            auto distance = xenon::distance2d( _ptr_acf->get_location(), agent.vcl_condition.location);                
            if ( distance <= MIN_ALLOWABLE_TAXING_DISTANCE ) {
                _next_action.action = ACF_DOES_WAITING_PUSH_BACK;
                break;
            }
        }        
    }
    
    _environment->agents_mutex.unlock();
    
}


