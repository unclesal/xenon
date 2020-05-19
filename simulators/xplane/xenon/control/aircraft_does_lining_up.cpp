// *********************************************************************************************************************
// *                                              Выравнивание на ВПП для взлета                                       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 19 may 2020 at 20:45 *
// *********************************************************************************************************************

#include "aircraft_does_lining_up.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesLiningUp::AircraftDoesLiningUp(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
): AircraftAbstractAction( ptr_acf, edge_d )
{
    __phase = PHASE_NOTHING;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Внутренний старт действия                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::_internal_start() {
    
    __phase = PHASE_STRAIGHT;
    // Поехали потихоньку.
    _params.acceleration = 0.0;
    _params.tug = TAXI_SLOW_TUG;
    _params.target_acceleration = TAXI_SLOW_ACCELERATION;
    _params.target_speed = TAXI_SLOW_SPEED;
    // Причем поехали пока что прямо, как стоим.
    _params.heading_acceleration = 0.0;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             "Шаг" в прямолинейной фазе                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::__step_straight( const float & elapsed_since_last_time ) {
    // Подруливание на точку осуществляем - сразу же, в "прямолинейной" фазе.
    auto wp = _get_front_wp();        
    auto bearing = xenon::bearing( _get_acf_location(), wp.location );
    auto heading = _get_acf_rotation().heading;
    auto delta = bearing - heading;        
    _params.target_heading = bearing;
    _params.heading_acceleration = 25.0 * delta * elapsed_since_last_time;
    
    double distance = _calculate_distance_to_wp( wp );    
    if ( distance < 5.0 ) {
        __phase = PHASE_ROTATION;
        
        _front_wp_reached();
        // И здесь же устанавливаем параметры изменения курса и торможения.
        // Это уже будет - дальняя точка рулежки.
        wp = _get_front_wp();
        bearing = xenon::bearing( _get_acf_location(), wp.location );
        heading = _get_acf_rotation().heading;
        delta = bearing - heading;        
        _params.target_heading = bearing;
        _params.heading_acceleration = 25.0 * delta * elapsed_since_last_time;
        
        _params.acceleration = 0.0;
        _params.tug = -0.1;
        _params.target_acceleration = -1.0;
        _params.target_speed = 0.0;
        
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Шаг в "фазе поворота"                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::__step_rotation( const float & elapsed_since_last_time ) {
    
    if ( _params.speed <= 0.2 ) {
        _params.speed = 0.0;
        _params.target_speed = 0.0;
        _params.tug = 0.0;
        _params.acceleration = 0.0;
    }
    
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Внутренний "шаг" действия                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::_internal_step( const float & elapsed_since_last_time ) {
        
    switch ( __phase ) {
        case PHASE_STRAIGHT: __step_straight( elapsed_since_last_time ); break;
        case PHASE_ROTATION: __step_rotation( elapsed_since_last_time ); break;
        default: XPlane::log("ERROR: AircraftDoesLiningUp::_internal_step(), unhandled phase " + to_string( __phase ) );
    }
    
}

