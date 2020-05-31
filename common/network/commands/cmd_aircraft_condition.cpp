// *********************************************************************************************************************
// *                                       Команда передачи по сети состояния самолета                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 30 may 2020 at 15:20 *
// *********************************************************************************************************************

#include "cmd_aircraft_condition.h"
#include "utils.hpp"

using namespace xenon;

// Чутка поэкономить на сетевом трафике.

constexpr uint16_t TAXI_LITES        = 1 << 0;
constexpr uint16_t LANDING_LITES     = 1 << 1;
constexpr uint16_t BEACON_LITES      = 1 << 2;
constexpr uint16_t STROBE_LITES      = 1 << 3;
constexpr uint16_t NAV_LITES         = 1 << 4;
constexpr uint16_t GEAR_DOWN         = 1 << 5;
constexpr uint16_t REVERSE_ON        = 1 << 6;


// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Пустой конструктор                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdAircraftCondition::CmdAircraftCondition()
    : CmdVehicleCondition()
{
    _command_name = "CmdAircraftCondition";
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Конструктор от параметров                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdAircraftCondition::CmdAircraftCondition(
    const vehicle_condition_t & vcl_condition, const aircraft_condition_t & acf_condition 
)   : CmdVehicleCondition( vcl_condition ) 
{
    _command_name = "CmdAircraftCondition";
    _acf_condition = acf_condition;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из объекта в JSON                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void CmdAircraftCondition::to_json( JSON & json ) {
    
    CmdVehicleCondition::to_json( json );
    
    // Вертикальная скорость, метров в секунду.
    json[ "vertical_speed" ] = _acf_condition.vertical_speed;
    json[ "target_vertical_speed" ] = _acf_condition.target_vertical_speed;
    // Вертикальное ускорение, метров в секунду.
    json[ "vertical_acceleration" ] = _acf_condition.vertical_acceleration;    

    // Угловые величины.        
    json[ "pitch_acceleration" ] = _acf_condition.pitch_acceleration;
    json[ "target_pitch" ] = _acf_condition.target_pitch;
    
    json[ "roll_acceleration" ] = _acf_condition.roll_acceleration;
    json[ "target_roll" ] = _acf_condition.target_roll;
    
    // Положение и состояние актуаторов.
    uint16_t actuators = 0;
    if ( _acf_condition.is_taxi_lites_on ) actuators &= TAXI_LITES;
    if ( _acf_condition.is_landing_lites_on ) actuators &= LANDING_LITES;
    if ( _acf_condition.is_beacon_lites_on ) actuators &= BEACON_LITES;
    if ( _acf_condition.is_strobe_lites_on ) actuators &= STROBE_LITES;
    if ( _acf_condition.is_nav_lites_on ) actuators &= NAV_LITES;
    if ( _acf_condition.is_gear_down ) actuators &= GEAR_DOWN;
    if ( _acf_condition.is_reverse_on ) actuators &= REVERSE_ON;
    json[ "actuators" ] = actuators;
    
    // Пропорциональные актуаторы.
    json[ "flaps_position" ] = _acf_condition.flaps_position;
    json[ "speed_brake_position" ] = _acf_condition.speed_brake_position;
    json[ "thrust_position" ] = _acf_condition.thrust_position;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из JSONа в объект                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void CmdAircraftCondition::from_json( JSON & json ) {
    
    CmdVehicleCondition::from_json( json );
    
    _acf_condition.vertical_speed = json.value( "vertical_speed", 0.0 );
    _acf_condition.target_vertical_speed = json.value( "target_vertical_speed", 0.0 );
    _acf_condition.vertical_acceleration = json.value( "vertical_acceleration", 0.0 );
    
    // Угловые величины.
    _acf_condition.pitch_acceleration = json.value( "pitch_acceleration", 0.0 );
    _acf_condition.target_pitch = json.value( "target_pitch", 0.0 );
    
    _acf_condition.roll_acceleration = json.value( "roll_acceleration", 0.0 );
    _acf_condition.target_roll = json.value( "target_roll", 0.0 );
    
    // Положение и состояние актуаторов.
    
    uint16_t actuators = json.value( "actuators", 0 );    
    _acf_condition.is_taxi_lites_on = actuators & TAXI_LITES;
    _acf_condition.is_landing_lites_on = actuators & LANDING_LITES;
    _acf_condition.is_beacon_lites_on = actuators & BEACON_LITES;
    _acf_condition.is_strobe_lites_on = actuators & STROBE_LITES;
    _acf_condition.is_nav_lites_on = actuators & NAV_LITES;
    _acf_condition.is_gear_down = actuators & GEAR_DOWN;
    _acf_condition.is_reverse_on = actuators & REVERSE_ON;
    
    // Пропорциональные актуаторы.
    
    _acf_condition.flaps_position = json.value( "flaps_position", 0.0 );
    _acf_condition.speed_brake_position = json.value( "speed_brake_position", 0.0 );
    _acf_condition.thrust_position = json.value( "thrust_position", 0.0 );
    
    // Считается на приемной стороне.
    _acf_condition.vertical_speed_fpm = xenon::meters_per_seconds_to_feet_per_min( _acf_condition.vertical_speed );
    
};
