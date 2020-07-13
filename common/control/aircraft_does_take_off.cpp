// *********************************************************************************************************************
// *                                                    Действие взлета                                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 20 may 2020 at 12:15 *
// *********************************************************************************************************************

#include "aircraft_does_take_off.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                      Конструктор                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************
AircraftDoesTakeOff::AircraftDoesTakeOff(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction( ptr_acf, edge_d )
{
    __phase = PHASE_NOTHING;
    __gear_up_altitude = 0.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Внутренний старт действия                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::_internal_start() {
    
    Logger::log( _ptr_acf->vcl_condition.agent_name + ", TO internal start");
    
    // Убираем точки из полетного плана, если они еще не были убраны.
        
    auto wp = _ptr_acf->flight_plan.get(0);
    while ( wp.type != WAYPOINT_RUNWAY && wp.action_to_achieve != ACF_DOES_TAKE_OFF ) {
        _ptr_acf->flight_plan.pop_front();
        wp = _ptr_acf->flight_plan.get(0);
    };
    
    __phase = PHASE_RUN_UP;
    
    __gear_up_altitude = 0.0;
    
    _ptr_acf->vcl_condition.is_clamped_to_ground = true;
    
    _ptr_acf->set_taxi_lites( false );
    _ptr_acf->set_landing_lites( true );
    _ptr_acf->set_beacon_lites( true );
    _ptr_acf->set_strobe_lites( true );
    _ptr_acf->set_nav_lites( true );
    
    // Резкое ускорение. На скорость воздействуем 
    // только один раз за весь взлет.
    
    _ptr_acf->vcl_condition.acceleration = 2.0;    
    _ptr_acf->vcl_condition.target_speed = knots_to_merets_per_second( _ptr_acf->parameters().v2 );
    
    _ptr_acf->vcl_condition.heading_acceleration = 0;
    _ptr_acf->vcl_condition.target_heading = _ptr_acf->vcl_condition.rotation.heading;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Один шаг в фазе разбега                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::__step__run_up( const float & elapsed_since_last_call ) {
    
    if ( _ptr_acf->vcl_condition.speed_kts > _ptr_acf->parameters().v1 ) {
        
        __phase = PHASE_BREAK_AWAY;        
        // Нос пошел вверх. Не слишком высоко, чтобы не царапал хвостом ВПП.
        _ptr_acf->acf_condition.target_pitch = _ptr_acf->parameters().take_off_angle;
        _ptr_acf->acf_condition.pitch_acceleration = 2.5f;
        
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Один шаг в фазе отрыва                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::__step__break_away( const float & elapsed_since_last_call ) {
    
    if ( _ptr_acf->vcl_condition.speed_kts >= _ptr_acf->parameters().v2 ) {
        
        __phase = PHASE_CLIMBING;
        _ptr_acf->vcl_condition.is_clamped_to_ground = false;
        
        // Нос пошел еще выше.
        _ptr_acf->acf_condition.target_pitch = 15.0f;
        _ptr_acf->acf_condition.pitch_acceleration = 0.8f;
        
        // Взлетаем. Скорость набора и вертикальное ускорение определяются только один раз.
        // И дальше он себе пошел вверх без ограничения по высоте, разбираться будем 
        // уже в действии полета (фаза нахождения в воздухе).

        _ptr_acf->acf_condition.vertical_acceleration = 0.6f;
        _ptr_acf->acf_condition.target_vertical_speed = feet_per_min_to_meters_per_second( _ptr_acf->parameters().vertical_climb_speed );
        
        // Высота, на которой включим уборку шасси.
#ifdef INSIDE_XPLANE
        auto position = _ptr_acf->get_position();
        __gear_up_altitude = position.y + 10.0;
#else
        auto location = _ptr_acf->get_location();
        __gear_up_altitude = location.altitude + 10.0;
#endif
        
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Один шаг в фазе набора высоты                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::__step__climbing( const float & elapsed_since_last_call ) {

    // Возможно, фаза взлета закончена и надо переходить на следующее действие.
    // Это определяется по расстоянию до дальней конечной точки ВПП.
    
    auto wp = _ptr_acf->flight_plan.get(0);
    auto distance = xenon::distance2d( _ptr_acf->get_location(), wp.location );
    if ( distance < 100.0 ) {
        Logger::log("Take off done");
        __phase = PHASE_NOTHING;
        _ptr_acf->flight_plan.pop_front();
        _finish();
        return;
    }
    
    // Если фаза еще не закончилась, то может быть надо убрать шасси.
    if ( __gear_up_altitude != 0.0 ) {
#ifdef INSIDE_XPLANE
        auto current_altitude = _ptr_acf->get_position().y;
#else
        auto current_altitude = _ptr_acf->get_location().altitude;
#endif
        if ( current_altitude >= __gear_up_altitude ) {
            // Поднимаем шасси и запоминаем, что мы его подняли,
            // чтобы больше не заходить в этот кусок кода и не проверять.
            _ptr_acf->set_gear_down( false );
            __gear_up_altitude = 0.0;
        };
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Внутренний шаг действия                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::_internal_step( const float & elapsed_since_last_call ) {
        
    auto wp = _ptr_acf->flight_plan.get( 0 );
    if ( wp.action_to_achieve != ACF_DOES_TAKE_OFF ) _ptr_acf->flight_plan.pop_front();
    
    // Подруливание по курсу осуществляется во всех фазах, цель - выйти как можно ближе к точке.

    _head_steering( elapsed_since_last_call, 20.0 );
    
    switch ( __phase ) {
        case PHASE_RUN_UP: __step__run_up( elapsed_since_last_call ); break;
        case PHASE_BREAK_AWAY: __step__break_away( elapsed_since_last_call ); break;
        case PHASE_CLIMBING: __step__climbing( elapsed_since_last_call ); break;
        default: Logger::log("ERROR: AircraftDoesTakeOff::_internal_step(), unhandled phase " + to_string( __phase ) );
    };
}



