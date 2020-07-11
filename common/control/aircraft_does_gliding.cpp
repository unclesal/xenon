// *********************************************************************************************************************
// *                            Заход в растр ILS, снижение. Фактически то же самое, что и посадка                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 11 jul 2020 at 09:08 *
// *********************************************************************************************************************
#include "aircraft_does_gliding.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesGliding::AircraftDoesGliding(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d
) : AircraftAbstractAction( ptr_acf, edge_d )
{

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Внутренний старт действия                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesGliding::_internal_start() {

    
    _ptr_acf->set_landing_lites( true );
    _ptr_acf->set_beacon_lites( true );
    _ptr_acf->set_nav_lites( true );
    _ptr_acf->set_beacon_lites( true );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Внутренний шаг действия                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesGliding::_internal_step(const float& elapsed_since_last_call) {
    
    auto wp = _ptr_acf->flight_plan.get(0);
    _head_bearing( wp );
    auto acf_location = _ptr_acf->get_location();
    
    auto distance = xenon::distance2d( acf_location, wp.location );
    if ( distance <= FLY_WAYPOINT_REACHED_DISTANCE ) {
        _ptr_acf->flight_plan.pop_front();
        _finish();
    };
    
    if ( _ptr_acf->vcl_condition.speed != 0.0 ) {
        auto time_to_achieve = distance / _ptr_acf->vcl_condition.speed;
        _altitude_adjustment( wp.location.altitude, time_to_achieve );
        _speed_adjustment( xenon::knots_to_merets_per_second( wp.speed ), time_to_achieve );
    }
    
    _control_of_flaps();
        
}


