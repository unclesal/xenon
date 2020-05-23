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
    for ( int i=0; i<PREVIOUS_ARRAY_SIZE; ++ i ) __previous_delta[i] = 0.0;        
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
    
    // Работа PID-регулятора, который устанавливает крен самолета. 
    // Сдвиг по курсу потом формируется - уже в зависимости от крена.
    float P = 1.0;
    float D = 0.0;
    float I = 1.0;
    
    float ivalue = 0.0;    
    for ( int i=0; i<PREVIOUS_ARRAY_SIZE; ++ i ) {
        ivalue += __previous_delta[i];
    };
    ivalue /= (float) PREVIOUS_ARRAY_SIZE;
    
    float dvalue = 0.0;
    for ( int i=0; i<PREVIOUS_ARRAY_SIZE - 1; ++i ) {
        dvalue += __previous_delta[i] - __previous_delta[i+1];
    };        
    
    float regulator_out = P * delta + D * dvalue + I * ivalue;
    
    if ( regulator_out >= 25.0 ) regulator_out = 25.0;
    if ( regulator_out <= -25.0 ) regulator_out = -25.0;
        
    _params.target_roll = regulator_out;
    delta < 0 ? _params.roll_acceleration = -3.0 : _params.roll_acceleration = 3.0;
        
    // Чтобы считать синусы-косинусы - надо иметь сдвинутый угол.
    // Повернутость угла влияет на знак, но он и так учитывается дальше.
    double radians = degrees_to_radians( rotation.roll - 90.0 );
    
    double dh = cos( radians );
    
    _params.target_heading = heading + dh * 10;
    _params.heading_acceleration = 10.0 * dh;
        
    for (int i = PREVIOUS_ARRAY_SIZE - 1 ; i >= 0; -- i ) {
        __previous_delta[i+1] = __previous_delta[i];
    };
    __previous_delta[0] = delta;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Внутренний "шаг" действия                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::_internal_step( const float & elapsed_since_last_call ) {
    __head_bearing( elapsed_since_last_call );
};
