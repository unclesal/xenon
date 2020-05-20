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
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Внутренний старт действия                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::_internal_start() {
    
    __phase = PHASE_RUN_UP;
    
    _ptr_acf->set_will_on_ground( true );
    
    _ptr_acf->set_taxi_lites( false );
    _ptr_acf->set_landing_lites( true );
    _ptr_acf->set_beacon_lites( true );
    _ptr_acf->set_strobe_lites( true );
    _ptr_acf->set_nav_lites( true );
    
    // Резкое ускорение. На скорость воздействуем 
    // только один раз за весь взлет.
    
    _params.tug = 0.4;
    _params.acceleration = 0.0;
    _params.target_acceleration = 2.0;
    _params.target_speed = 102.889; // 200 kph

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Один шаг в фазе разбега                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::__step__run_up( const float & elapsed_since_last_call ) {
    
    if ( _params.speed_kph > _get_acf_parameters().v1 ) {
        
        __phase = PHASE_BREAK_AWAY;        
        // Нос пошел вверх. Не слишком высоко, чтобы не царапал хвостом ВПП.
        _params.target_pitch = 9.0f;
        _params.pitch_acceleration = 2.5f;
        
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Один шаг в фазе отрыва                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::__step__break_away( const float & elapsed_since_last_call ) {
    
    if ( _params.speed_kph > _get_acf_parameters().v2 ) {
        
        __phase = PHASE_CLIMBING;
        _acf_will_on_ground( false );
        
        // Нос пошел еще выше.
        _params.target_pitch = 15.0f;
        _params.pitch_acceleration = 0.8f;
        
        // Взлетаем. Скорость набора и вертикальное ускорение определяются только один раз.
        // И дальше он себе пошел вверх без ограничения по высоте, разбираться будем 
        // уже в действии полета (фаза нахождения в воздухе).

        _params.vertical_acceleration = 0.6f;
        _params.target_vertical_speed = feet_per_min_to_meters_per_second( _get_acf_parameters().vertical_climb_speed );
        
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Один шаг в фазе набора высоты                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::__step__climbing( const float & elapsed_since_last_call ) {

//     auto position = _get_acf_position();
//     auto location = _get_acf_location();
//     
//     XPlane::log(
//         "va=" + to_string( _params.vertical_acceleration ) + ", vs=" + to_string( _params.vertical_speed )
//         + ", y=" + to_string( position.y ) + ", alt=" + to_string( location.altitude )
//         + ", on_ground=" + to_string( _will_acf_on_ground() ) 
//     );
    
    auto wp = _get_front_wp();    
    auto distance = xenon::distance( _get_acf_location(), wp.location );
    if ( distance < 100.0 ) {
        XPlane::log("Take off done");
        __phase = PHASE_NOTHING;
        _finish();
        return;
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Внутренний шаг действия                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTakeOff::_internal_step( const float & elapsed_since_last_call ) {
        
    // Подруливание по курсу осуществляется во всех фазах, цель - выйти как можно ближе к точке.
    _head_steering( elapsed_since_last_call, 20.0 );
    switch ( __phase ) {
        case PHASE_RUN_UP: __step__run_up( elapsed_since_last_call ); break;
        case PHASE_BREAK_AWAY: __step__break_away( elapsed_since_last_call ); break;
        case PHASE_CLIMBING: __step__climbing( elapsed_since_last_call ); break;
        default: XPlane::log("ERROR: AircraftDoesTakeOff::_internal_step(), unhandled phase " + to_string( __phase ) );
    };
}



