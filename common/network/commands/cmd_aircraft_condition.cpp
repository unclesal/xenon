// *********************************************************************************************************************
// *                                       Команда передачи по сети состояния самолета                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 30 may 2020 at 15:20 *
// *********************************************************************************************************************

#include "cmd_aircraft_condition.h"
#include "utils.hpp"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Пустой конструктор                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdAircraftCondition::CmdAircraftCondition()
    : CmdVehicleCondition()
{
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
    json[ "is_taxi_lites_on" ] = _acf_condition.is_taxi_lites_on;
    json[ "is_landing_lites_on" ] = _acf_condition.is_landing_lites_on;
    json[ "is_beacon_lites_on" ] = _acf_condition.is_beacon_lites_on;
    json[ "is_strobe_lites_on" ] = _acf_condition.is_strobe_lites_on;
    
//     bool is_nav_lites_on = false;
//     bool is_gear_down = false;
//     bool is_reverse_on = false;
//     float flaps_position = 0.0;
//     float speed_brake_position = 0.0;
//     float thrust_position = 0.0;

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
    _acf_condition.is_taxi_lites_on = json.value( "is_taxi_lites_on", false );
    _acf_condition.is_landing_lites_on = json.value( "is_landing_lites_on", false );
    _acf_condition.is_beacon_lites_on = json.value( "is_beacon_lites_on", false );
    _acf_condition.is_strobe_lites_on = json.value( "is_strobe_lites_on", false );
    
    // Считается на приемной стороне.
    _acf_condition.vertical_speed_fpm = xenon::meters_per_seconds_to_feet_per_min( _acf_condition.vertical_speed );
    
};

