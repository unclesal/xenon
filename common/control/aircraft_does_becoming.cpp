// *********************************************************************************************************************
// *                  "Становление" самолета, переход из состояния "взлетел" в состояние "в полете"                    *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 11 jul 2020 at 11:03 *
// *********************************************************************************************************************
#include "aircraft_does_becoming.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Конструктор                                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesBecoming::AircraftDoesBecoming( 
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d
) : AircraftAbstractAction( ptr_acf, edge_d )
{
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Внутренний старт действия                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesBecoming::_internal_start() {
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Внутренний шаг действия                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void xenon::AircraftDoesBecoming::_internal_step( const float & elapsed_since_last_call ) {
    
    auto location = _ptr_acf->get_location();
    auto wp = _ptr_acf->flight_plan.get(0);
    _head_bearing( wp );
    
    _control_of_flaps();
    
    auto distance = xenon::distance2d(location, wp.location );
    if ( distance <= FLY_WAYPOINT_REACHED_DISTANCE ) {
        _ptr_acf->flight_plan.pop_front();
        _finish();
    };
    
    if ( _ptr_acf->vcl_condition.speed ) {
      
        auto time_to_achieve = distance / _ptr_acf->vcl_condition.speed;
        
        auto target_speed = xenon::knots_to_merets_per_second( wp.speed );
        _speed_adjustment( target_speed, time_to_achieve );
    
        auto target_altitude = wp.location.altitude;
        _altitude_adjustment( target_altitude, time_to_achieve );
    };
}



