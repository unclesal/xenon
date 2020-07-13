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
    
    Logger::log( _ptr_acf->vcl_condition.agent_name + ", LU internal start");
    __phase = PHASE_STRAIGHT;
    // Поехали потихоньку.
        
    _ptr_acf->vcl_condition.acceleration = TAXI_NORMAL_ACCELERATION;
    _ptr_acf->vcl_condition.target_speed = TAXI_NORMAL_SPEED;
    
    // Причем поехали пока что прямо, как стоим.
    _ptr_acf->vcl_condition.heading_acceleration = 0.0;
    
    _ptr_acf->set_taxi_lites( false );    
    _ptr_acf->set_landing_lites( true );    
    _ptr_acf->set_beacon_lites( true );
    _ptr_acf->set_strobe_lites( true );
    _ptr_acf->set_nav_lites( true );
    
    // Типа "взлетное положение"
    _ptr_acf->set_flaps_position( _ptr_acf->parameters().flaps_take_off_position );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             "Шаг" в прямолинейной фазе                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::__step_straight( const float & elapsed_since_last_call ) {
    
    // Подруливание на точку осуществляем - сразу же, в "прямолинейной" фазе.
    // _head_steering( elapsed_since_last_call, 25.0 );

    auto wp = _ptr_acf->flight_plan.get(0);
    double distance = xenon::distance2d( _ptr_acf->get_location(), wp.location );
    
    if (( distance < 75.0 ) && ( _ptr_acf->vcl_condition.target_speed != TAXI_SLOW_SPEED )) {
        _taxi_breaking( TAXI_SLOW_SPEED, 3.0 );        
    };
    
    if ( _taxi_turn_started( wp ) ) {        
        __phase = PHASE_ROTATION;
        // Убираем ближнюю точку ВПП, мы ее достигли.
        _ptr_acf->flight_plan.pop_front();
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Шаг в "фазе поворота"                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::__step_rotation( const float & elapsed_since_last_call ) {
    
    auto wp = _ptr_acf->flight_plan.get(0);
    auto bearing = xenon::bearing( _ptr_acf->get_location(), wp.location );
    auto heading = _ptr_acf->get_rotation().heading;
    auto delta = bearing - heading; 
    
//     Logger::log(
//         "Bearing=" + to_string(bearing) + ", heading=" + to_string( heading ) + ", delta=" + to_string( delta )
//     );
    
    if ( abs(delta) < 5.0 ) {  // && ( _ptr_acf->vcl_condition.target_speed != 0.0 ) ) {
        // Тормозим.        
        _taxi_breaking( 0.0, 2.0 );
        // И фиксируем текущий курс, больше крутиться не будем.
        _ptr_acf->vcl_condition.heading_acceleration = 0.0;

    };
    
    if ( _ptr_acf->vcl_condition.speed <= 0.5 ) {
        
        _ptr_acf->vcl_condition.speed = 0.0;
        _ptr_acf->vcl_condition.target_speed = 0.0;        
        _ptr_acf->vcl_condition.acceleration = 0.0;
                
        // Убираем точки из полетного плана, если они еще не были убраны.
        
        wp = _ptr_acf->flight_plan.get(0);
        while ( wp.type != WAYPOINT_RUNWAY && wp.action_to_achieve != ACF_DOES_TAKE_OFF ) {
            _ptr_acf->flight_plan.pop_front();
            wp = _ptr_acf->flight_plan.get(0);
        };
        
        _finish();
    }
    
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Внутренний "шаг" действия                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLiningUp::_internal_step( const float & elapsed_since_last_call ) {
    
    /*
    Logger::log( 
        _ptr_acf->vcl_condition.agent_name + ", LU accel=" + to_string( _ptr_acf->vcl_condition.acceleration )
        + ", speed=" + to_string( _ptr_acf->vcl_condition.speed )
    );
    */
    
    /*
    // Видимо, оно после старта может быть сбито фреймом? Не совсем понял, что происходит, сделал - "костыль".
    if ( ( !  _ptr_acf->vcl_condition.acceleration ) || ( ! _ptr_acf->vcl_condition.target_speed ) ) {
        _ptr_acf->vcl_condition.acceleration = TAXI_NORMAL_ACCELERATION;
        _ptr_acf->vcl_condition.target_speed = TAXI_NORMAL_SPEED;
    };
    */
    
    if ( __phase == PHASE_NOTHING ) __phase = PHASE_STRAIGHT;
    
    switch ( __phase ) {
        case PHASE_STRAIGHT: __step_straight( elapsed_since_last_call ); break;
        case PHASE_ROTATION: __step_rotation( elapsed_since_last_call ); break;
        default: Logger::log("ERROR: AircraftDoesLiningUp::_internal_step(), unhandled phase " + to_string( __phase ) );
    }
    
}

