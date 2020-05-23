// *********************************************************************************************************************
// *                                          Самолет выполняет действие полета                                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 14:24 *
// *********************************************************************************************************************

#include "aircraft_does_flying.h"

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesFlying::AircraftDoesFlying(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction( ptr_acf, edge_d )
{
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Внутренний старт действия                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::_internal_start() {
    XPlane::log("FLY: start");
    __previous_delta = 0.0;    
    __step = 0;
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             "Подруливание" на курс - в авиационной реализации, не в колесной                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::__head_bearing( const float & elapsed_since_last_call ) {
    
    auto rotation = _get_acf_rotation();
    
    auto wp = _get_front_wp();
    if ( wp.type == WAYPOINT_UNKNOWN ) {
        XPlane::log("ERROR: AircraftDoesFlying::__head_bearing(), type of front FP waypoint is UNKNOWN...");
        return;
    };
    
    auto location = _get_acf_location();
    float heading = _get_acf_rotation().heading;
    float bearing = xenon::bearing( location, wp.location );
    auto delta = bearing - heading;
    
    delta < 0 ? _params.target_roll = -25.0 : _params.target_roll = 25.0;
    
    // Работа PID-регулятора, который устанавливает крен самолета. 
    // Сдвиг по курсу потом формируется - уже в зависимости от крена.
    float P = 2.0;
    float D = 5.0;
    float I = 2.0;
    
    float regulator_out = P * delta + D * ( delta - __previous_delta ) + I * ( delta + __previous_delta );
    
    if ( regulator_out >= 10.0 ) regulator_out = 10.0;
    if ( regulator_out <= -10.0 ) regulator_out = -10.0;
    
    // rotation.roll += regulator_out;    
    // if ( rotation.roll < -25.0 ) rotation.roll = -25.0;
    // if ( rotation.roll > 25.0 ) rotation.roll = 25.0;    
    // _set_acf_rotation( rotation );
    
    _params.roll_acceleration = regulator_out;
    
    // Чтобы считать синусы-косинусы - надо иметь сдвинутый и повернутый угол.
    double radians = degrees_to_radians( rotation.roll - 90.0 );
    
    double dh = cos( radians );
    
    XPlane::log(
        "heading=" + to_string(heading) + ", delta=" + to_string(delta) 
        + ", p=" + to_string( P * delta )
        + ", diff=" + to_string( D * (delta - __previous_delta ) ) 
        + ", integral=" + to_string( I * (delta + __previous_delta ) ) 
        + ", reg_out=" + to_string( regulator_out )
        + ", roll=" + to_string( rotation.roll )
    );
    
    _params.target_heading = heading + dh * 10;
    _params.heading_acceleration = 10.0 * dh;
    
    // __step ++;
    // if ( __step >= 4  ) {
    //    __step = 0;
        __previous_delta = delta;
    // }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Внутренний "шаг" действия                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::_internal_step( const float & elapsed_since_last_call ) {
    __head_bearing( elapsed_since_last_call );
};
