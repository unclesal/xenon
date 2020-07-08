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
    _next_action.action = ACF_DOES_NOTHING;
    _next_action.priority = 20;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Обновление состояния фрейма                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void TaxingDistance::update( CmdAircraftCondition * cmd ) {
    
    _next_action.action = ACF_DOES_NOTHING;
    
    _environment->agents_mutex.lock();
    
    auto our_location = _ptr_acf->get_location();
    auto our_heading = _ptr_acf->vcl_condition.rotation.heading;
    
    for ( auto agent: _environment->agents ) {
        if ( agent.is_aircraft() && agent.is_on_taxiway() ) {
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
            
            if (( delta >= 270 ) || ( delta <= 90 )) {
                // Данный агент-самолет - впереди по курсу. Смотрим на дистанцию.
                auto distance = xenon::distance2d( our_location, his_location );
                if ( distance <= MIN_ALLOWABLE_TAXING_DISTANCE ) {
                    _next_action.action = ACF_DOES_TAXING_STOP;
                    Logger::log(
                        "TaxingDistance::stop here, heading=" + to_string(our_heading) 
                        + ", name=" + agent.vcl_condition.agent_name
                        + ", bearing=" + to_string( bearing ) 
                        + ", delta=" + to_string( delta ) 
                        + ", distance=" + to_string(distance)
                    );
                    break;
                }
            };
        };
    }
        
    _environment->agents_mutex.unlock();
}

