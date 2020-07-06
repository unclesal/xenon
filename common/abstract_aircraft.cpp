// *********************************************************************************************************************
// *                                       An abstract aircraft internal of X-Plane simulator.                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 mar 2019 at 15:47 *
// *********************************************************************************************************************

#include "abstract_aircraft.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   The constructor                                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

AbstractAircraft::AbstractAircraft()
    : AbstractVehicle()
{
    // Это уже для самолета. Он по умолчанию к земле - не прижат.
    vcl_condition.is_clamped_to_ground = false;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Изменение состояния самолета от пришедшей структуры                                *
// *                                                                                                                   *
// *********************************************************************************************************************

void AbstractAircraft::update_from( const vehicle_condition_t & vc, const aircraft_condition_t & ac ) {
    
    AbstractVehicle::update_from( vc );
    acf_condition = ac;
#ifdef INSIDE_XPLANE

    set_taxi_lites( acf_condition.is_taxi_lites_on );
    set_landing_lites( acf_condition.is_landing_lites_on );
    set_beacon_lites( acf_condition.is_beacon_lites_on );
    set_strobe_lites( acf_condition.is_strobe_lites_on );
    set_nav_lites( acf_condition.is_nav_lites_on );
    set_gear_down( acf_condition.is_gear_down );
    set_reverse_on( acf_condition.is_reverse_on );
    
    set_flaps_position( acf_condition.flaps_position );
    set_speed_brake_position( acf_condition.speed_brake_position );
    set_thrust_position( acf_condition.thrust_position );

#endif    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Преобразование объекта в JSON                                           *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void AbstractAircraft::to_json( JSON & json ) {
    AircraftState::to_json( json );
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Преобразование из JSONа в объект.                                        *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void AbstractAircraft::from_json( JSON & json) {
    AircraftState::from_json( json );
}
*/
