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
    XPlane::log("FLY: start");
    for ( int i=0; i<PREVIOUS_ARRAY_SIZE; ++ i ) __previous_delta[i] = 0.0;
    
    auto wp = _get_front_wp();
    if ( wp.type == WAYPOINT_UNKNOWN ) {
        XPlane::log("AircraftDoesFlying::_internal_start(), WP0 have UNKNOWN type.");
        return;
    };
    
    if ( wp.location.altitude != 0.0f ) __phase = PHASE_WAYPOINT_CONTROLLED;
    else __phase = PHASE_CLIMBING;
    
    // Контроль скорости будет осуществляться в данном действии 
    // автоматически, поэтому существующие значения - обнуляем.
    _params.tug = 0.0;
    _params.acceleration = 0.0;
    
    // -----------------------------------------------------------------------
    // TODO: убрать потом. Для отладки: какие-то похожие на правду скорости.
    _params.tug = 0.4;
    _params.acceleration = 0.0;
    _params.target_acceleration = 2.0;
    _params.target_speed = 102.889; // 200 kph
    _params.speed = 100.0;
    
    _params.vertical_acceleration = 0.6f;
    _params.target_vertical_speed = feet_per_min_to_meters_per_second( _get_acf_parameters().vertical_climb_speed );
    // -----------------------------------------------------------------------

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
    
    // Этот коэффициент не сильно важен. Важно, чтобы курс изменился.
    _params.target_heading = heading + dh * 10;
    
    // А этот коэффициент определяет скорость вращения в воздухе.
    // Подобран исходя из зрительного восприятия картинки.
    _params.heading_acceleration = 15.0 * dh;
        
    for (int i = PREVIOUS_ARRAY_SIZE - 1 ; i >= 0; -- i ) {
        __previous_delta[i+1] = __previous_delta[i];
    };
    __previous_delta[0] = delta;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                      Установка высоты в зависимости от фазы и следующей контрольной точки.                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesFlying::__control_of_altitude( 
    const waypoint_t & waypoint, const float & time_to_achieve
) {
    
    auto location = _get_acf_location();
    auto current_altitude = location.altitude;
    auto acf_parameters = _get_acf_parameters();
    auto acf_rotation = _get_acf_rotation();        
    
    if ( ( __phase == PHASE_WAYPOINT_CONTROLLED ) && ( waypoint.location.altitude != 0.0f ) ) {
        
        // Есть смысл говорить о выходе на высоту данной контрольной точки.
        
        auto target_altitude = waypoint.location.altitude;
        auto da = target_altitude - current_altitude;        
        
        // Сначала обнуляем параметры изменения высоты, т.к. пока еще непонятно,
        // в каком положении мы сейчас находимся, выше или ниже целевого значения.
        _params.pitch_acceleration = 0.0;
        _params.target_pitch = 0.0;
        
        _params.vertical_acceleration = 0.0;
        _params.target_vertical_speed = 0.0;
        
        // Вертикальная скорость выбирается таким образом, чтобы 
        // достигнуть нужной нам высоты прямо на точке привода.
        // Несколько "не кошерно" в том плане, что вертикальная
        // скорость может измениться рывком. Но видно этого не будет
        // при любом раскладе событий, так что вполне допустимо.
        
        _params.vertical_speed = 0.0;
        if ( time_to_achieve ) _params.vertical_speed = da / time_to_achieve;
        
        // Целевое значение тангажа.
        if ( da > 0.0 ) {            
            // Мы находимся - ниже, надо подниматься.
            _params.target_pitch = 5.0;                                    
        } else if ( da < 0.0 ) {            
            // Мы находимся - выше, надо опускаться.            
            _params.target_pitch = -5.0;                        
        }
        
        // Изменение тангажа. А вот тангаж можно увидеть. 
        // Соответственно, резко изменяться он не может.
        if ( acf_rotation.pitch > _params.target_pitch ) _params.pitch_acceleration = -1.0f;
        else _params.pitch_acceleration = 1.0f;
    
//         XPlane::log(
//             "alt=" + to_string( location.altitude ) + ", target=" + to_string(target_altitude)
//             + ", da=" + to_string( da ) + ", time=" + to_string( time_to_achieve )
//             + ", vs=" + to_string( _params.vertical_speed )
//             + ", va=" + to_string( _params.vertical_acceleration )
//             + ", ts=" + to_string( _params.target_vertical_speed )
//             + ", pitch = " + to_string( _get_acf_rotation().pitch )
//             + ", accel=" + to_string( _params.acceleration )
//             + ", speed=" + to_string( _params.speed )
//         );
        
    } else {
        // Либо фаза не та, либо у данной контрольной точки нет высоты.
        XPlane::log(
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
        
        auto target_speed = xenon::knodes_to_merets_per_second( waypoint.speed );
        auto ds = target_speed - _params.speed;
        
        _params.target_speed = target_speed;
        _params.acceleration = 0.0;
        if ( time_to_achieve != 0.0 ) _params.acceleration = ds / time_to_achieve;
        
//         XPlane::log(
//             "Speed=" + to_string(_params.speed) + ", target=" + to_string( target_speed )
//             + ", ds=" + to_string( ds ) + ", achieve=" + to_string( time_to_achieve )
//             + ", accel=" + to_string( _params.acceleration )
//         );

    } else {
        XPlane::log(
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
    
    __head_bearing( elapsed_since_last_call );
    
    auto wp = _get_front_wp();
    auto distance = xenon::distance( _get_acf_location(), wp.location );
    
    // XPlane::log( to_string(distance) );
    if ( distance <= 1000.0 ) {
        XPlane::log("Front WP reached, distance=" + to_string( distance ) );
        _front_wp_reached();
        wp = _get_front_wp();
        if ( wp.type == WAYPOINT_UNKNOWN ) {
            XPlane::log("ERROR: AircraftDoesFlying::_internal_step(), after WP reached next WP type is UNKNOWN.");
            return;
        };
        // Переставляем фазу в зависимости от полученной следующей точки.
        if ( wp.location.altitude != 0.0f ) __phase = PHASE_WAYPOINT_CONTROLLED;
        else {
            // TODO: Самостоятельная установка фаз в зависимости от высоты            
            XPlane::log("UNRELEASED: set next phase for flying.");
        };
    };
    
    float time_to_achieve = 0.0;
    if ( _params.speed ) time_to_achieve = distance / _params.speed;
    
    __control_of_altitude( wp, time_to_achieve );
    __control_of_speed( wp, time_to_achieve );
};
