// *********************************************************************************************************************
// *                                          Самолет выполняет действие полета                                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 14:24 *
// *********************************************************************************************************************

#include "aircraft_does_flying.h"
#include "utils.hpp"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesFlying::AircraftDoesFlying(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction( ptr_acf, edge_d )
{
    __phase = PHASE_UNKNOWN;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Внутренний старт действия                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::_internal_start() {
    
    auto wp = _get_front_wp();
    if ( wp.type == WAYPOINT_UNKNOWN ) {
        Logger::log("AircraftDoesFlying::_internal_start(), WP0 have UNKNOWN type.");
        return;
    };
    
    if ( wp.location.altitude != 0.0f ) __phase = PHASE_WAYPOINT_CONTROLLED;
    else __phase = PHASE_CLIMBING;
    
    Logger::log("FLYING start, name=" + wp.name + ", alt=" + to_string(wp.location.altitude) + ", phase=" + to_string( __phase ));
    
    // Контроль скорости будет осуществляться в данном действии 
    // автоматически, поэтому существующие значения - обнуляем.
    // _ptr_acf->vcl_condition.acceleration = 0.0;
    
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                      Установка высоты в зависимости от фазы и следующей контрольной точки.                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::__control_of_altitude( 
    const waypoint_t & waypoint, const float & time_to_achieve
) {
        
    if ( ( __phase == PHASE_WAYPOINT_CONTROLLED ) && ( waypoint.location.altitude != 0.0f ) ) {
        
        // Есть смысл говорить о выходе на высоту данной контрольной точки.
        
        auto target_altitude = waypoint.location.altitude;
        _altitude_adjustment( target_altitude, time_to_achieve );
                    
    } else {
        // Либо фаза не та, либо у данной контрольной точки нет высоты.
        Logger::log(
            "UNRELEASED: AircraftDoesFlying::__control_of_altitude(), phase=" + to_string( __phase ) 
            + ", wp.altitude=" + to_string( waypoint.location.altitude ) 
        );
    }
        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                    Установка скорости в зависимости от фазы и следующей контрольной точки.                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::__control_of_speed (
    const waypoint_t & waypoint, const float & time_to_achieve 
) {
    
    if ( ( __phase == PHASE_WAYPOINT_CONTROLLED ) && ( waypoint.speed != 0.0 ) ) {
        
        auto target_speed = xenon::knots_to_merets_per_second( waypoint.speed );
        _speed_adjustment( target_speed, time_to_achieve );
        
    } else {
        Logger::log(
            "UNRELEASED: AircraftDoesFlying::__control_of_speed(), phase=" + to_string( __phase )
            + ", wp speed=" + to_string( waypoint.speed )
        );
    };    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Внутренний "шаг" действия                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::_internal_step( const float & elapsed_since_last_call ) {
    
    // Положение закрылков в полете.
    auto acf_params = _get_acf_parameters();
    
    if (
        ( _ptr_acf->vcl_condition.speed >= xenon::knots_to_merets_per_second( acf_params.flaps_take_off_speed ))
        && ( _ptr_acf->vcl_condition.acceleration >= 0.0 )
        && ( _ptr_acf->acf_condition.flaps_position != 0.0 )
    ) {
        _ptr_acf->set_flaps_position(0.0);
        XPlane::log("FLY: laps to 0");
    }
    
    if ( 
        ( _ptr_acf->vcl_condition.speed <= xenon::knots_to_merets_per_second( acf_params.flaps_take_off_speed))
        && ( _ptr_acf->vcl_condition.acceleration <= 0.0 )
        && ( _ptr_acf->acf_condition.flaps_position != acf_params.flaps_take_off_position )
    ) {
        _ptr_acf->set_flaps_position( acf_params.flaps_take_off_position );
        XPlane::log("FLY: flaps to TO");
    };
    
    auto wp = _get_front_wp();
    _head_bearing( wp );
    
    // Logger::log("After bearing phase=" + to_string( __phase ) );    
    
    auto distance = xenon::distance2d( _get_acf_location(), wp.location );
        
    // Logger::log( "after distance phase=" + to_string( __phase ) + ", distance=" + to_string(distance) );
    
    if ( distance <= 1000.0 ) {
        Logger::log("Does FLYING, " + wp.name + " reached, distance=" + to_string( distance ) );
        _front_wp_reached();
        
        // Следующая точка полетного плана для определения, что делать дальше.
        wp = _get_front_wp();
        Logger::log(
            "Does FLYING, next wp: " + wp.name + ", alt=" + to_string( wp.location.altitude ) 
            + ", type=" + to_string( wp.type ) + ", action=" + to_string( wp.action_to_achieve )
        );
        
        if ( wp.type == WAYPOINT_UNKNOWN ) {
            Logger::log("ERROR: AircraftDoesFlying::_internal_step(), after WP reached next WP type is UNKNOWN.");
            return;
        };
        
        if ( wp.action_to_achieve == ACF_DOES_LANDING ) {
            // Все. Прилетели. Передаем управление состоянию "на посадочной 
            // прямой" и через него - действию посадки.
            _finish();
            return;
        };
        
        // Переставляем фазу в зависимости от полученной следующей точки.
        if ( wp.location.altitude != 0.0f ) __phase = PHASE_WAYPOINT_CONTROLLED;
        else {
            // TODO: Самостоятельная установка фаз в зависимости от высоты
            Logger::log("UNRELEASED: set next phase for flying.");
        };        
    };
    
    float time_to_achieve = 0.0;
    if ( _ptr_acf->vcl_condition.speed ) time_to_achieve = distance / _ptr_acf->vcl_condition.speed;
    
    __control_of_altitude( wp, time_to_achieve );
    __control_of_speed( wp, time_to_achieve );
};
