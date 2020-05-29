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
    
    // _ptr_acf->condition.tug = TAXI_NORMAL_TUG;
    // _ptr_acf->vcl_condition.target_acceleration = TAXI_NORMAL_ACCELERATION;
    
    _ptr_acf->vcl_condition.acceleration = 0.0;        
    _ptr_acf->vcl_condition.target_speed = TAXI_SLOW_SPEED;
    
    // Причем поехали пока что прямо, как стоим.
    _ptr_acf->vcl_condition.heading_acceleration = 0.0;
    
    _ptr_acf->set_taxi_lites( false );    
    _ptr_acf->set_landing_lites( true );    
    _ptr_acf->set_beacon_lites( true );
    _ptr_acf->set_strobe_lites( true );
    _ptr_acf->set_nav_lites( true );
    
    // Типа "взлетное положение"
    _ptr_acf->set_flaps_position( _get_acf_parameters().flaps_take_off_position );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             "Шаг" в прямолинейной фазе                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::__step_straight( const float & elapsed_since_last_call ) {
    
    // Подруливание на точку осуществляем - сразу же, в "прямолинейной" фазе.
    _head_steering( elapsed_since_last_call, 25.0 );

    auto wp = _get_front_wp();
    double distance = _calculate_distance_to_wp( wp );        
    if ( distance < 8.0 ) {
        __phase = PHASE_ROTATION;
        // Убираем ближнюю точку ВПП, мы ее достигли.
        _front_wp_reached();
        
        // Чуть начинаем подтормаживать.
        _ptr_acf->vcl_condition.acceleration = 0.0;
        _ptr_acf->vcl_condition.target_speed = 2.0;

        // _ptr_acf->condition.tug = -0.2;
        // _ptr_acf->vcl_condition.target_acceleration = -2.0;
        
        
        // И здесь же устанавливаем параметры изменения курса и торможения.
        // Это уже будет - дальняя точка рулежки.
        
        _head_steering( elapsed_since_last_call, 25.0 );                
                
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Шаг в "фазе поворота"                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::__step_rotation( const float & elapsed_since_last_call ) {
    
    auto wp = _get_front_wp();
    auto bearing = xenon::bearing( _get_acf_location(), wp.location );
    auto heading = _get_acf_rotation().heading;
    auto delta = bearing - heading; 
    
    if ( ( abs(delta) < 5.0 ) && ( _ptr_acf->vcl_condition.target_speed != 0.0 ) ) {
        // Тормозим.
        
        // _ptr_acf->condition.tug = -0.2;
        // _ptr_acf->vcl_condition.target_acceleration = -2.0;
        
        _ptr_acf->vcl_condition.acceleration = 0.0;        
        _ptr_acf->vcl_condition.target_speed = 0.0;

    } else {
        // Все еще выполняем поворот.
        _head_steering( elapsed_since_last_call, 25.0 );
    };
    
    if ( _ptr_acf->vcl_condition.speed <= 0.2 ) {
        _ptr_acf->vcl_condition.speed = 0.0;
        _ptr_acf->vcl_condition.target_speed = 0.0;
        // _ptr_acf->condition.tug = 0.0;
        _ptr_acf->vcl_condition.acceleration = 0.0;
        _finish();
    }
    
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Внутренний "шаг" действия                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::_internal_step( const float & elapsed_since_last_call ) {
    
    switch ( __phase ) {
        case PHASE_STRAIGHT: __step_straight( elapsed_since_last_call ); break;
        case PHASE_ROTATION: __step_rotation( elapsed_since_last_call ); break;
        default: Logger::log("ERROR: AircraftDoesLiningUp::_internal_step(), unhandled phase " + to_string( __phase ) );
    }
    
}

