// *********************************************************************************************************************
// *                          Фрейм, отслеживающий соблюдение дистанции между самолетами при рулении                   *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 08 jul 2020 at 17:12 *
// *********************************************************************************************************************
#include "taxing_distance.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

TaxingDistance::TaxingDistance(
    BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment
) : StateFrame( bimbo, environment ) 
{    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Обновление состояния фрейма                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void TaxingDistance::update() {
    
    _before_update();
    
    _environment->agents_mutex.lock();
        
    for ( auto agent: _environment->agents ) {
        if ( agent.is_aircraft() && agent.is_on_taxiway() ) {
            
            auto his_location = agent.vcl_condition.location;
            auto delta = xenon::course_to( _our_location, _our_rotation.heading, his_location );
                        
            if (( delta >= 270 ) || ( delta <= 90 )) {
                // Данный агент-самолет - впереди по курсу. Смотрим на дистанцию.
                auto distance = xenon::distance2d( _our_location, his_location );
                if ( distance <= MIN_ALLOWABLE_TAXING_DISTANCE ) {
                    _next_action.action = ACF_DOES_TAXING_STOP;
                    _activated = true;
                    
                    /*
                    Logger::log(
                        "TaxingDistance: " + _ptr_acf->vcl_condition.agent_name + " -> " + agent.vcl_condition.agent_name + ", dis=" + std::to_string( distance )
                    );
                    */
                    
                    break;
                }
            };
        };
    }
        
    _environment->agents_mutex.unlock();
}

