// *********************************************************************************************************************
// *                                       Команда передачи по сети состояния самолета                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 30 may 2020 at 15:20 *
// *********************************************************************************************************************

#include "cmd_aircraft_condition.h"
#include "utils.hpp"
#include "tested_agents.h"

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

    // Поля, которые могут быть пустыми.
    if ( ! _acf_condition.icao_type.empty()) json[ "icao_type" ] = _acf_condition.icao_type;
    if ( ! _acf_condition.icao_airline.empty()) json[ "icao_airline" ] = _acf_condition.icao_airline;
    if ( ! _acf_condition.livery.empty()) json[ "livery" ] = _acf_condition.livery;

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
    if ( _acf_condition.is_taxi_lites_on ) actuators |= TAXI_LITES;
    if ( _acf_condition.is_landing_lites_on ) actuators |= LANDING_LITES;
    if ( _acf_condition.is_beacon_lites_on ) actuators |= BEACON_LITES;
    if ( _acf_condition.is_strobe_lites_on ) actuators |= STROBE_LITES;
    if ( _acf_condition.is_nav_lites_on ) actuators |= NAV_LITES;
    if ( _acf_condition.is_gear_down ) actuators |= GEAR_DOWN;
    if ( _acf_condition.is_reverse_on ) actuators |= REVERSE_ON;
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
    
    _acf_condition.icao_type = json.value( "icao_type", "" );
    _acf_condition.icao_airline = json.value( "icao_airline", "" );
    _acf_condition.livery = json.value( "livery", "" );
    
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

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             Перекрытая процедура выполнения команды на стороне сервера.                           *
// *                Часть информации не передается по сети, т.к. сервер знает об этом из базы данных.                  *
// *                                                                                                                   *
// *********************************************************************************************************************
#ifdef SERVER_SIDE
void CmdAircraftCondition::execute_on_server( ConnectedClientCore * client, ClientsListener * server ) {
    
    // Заполнение полей происходит - до вызова "родительской" функции, 
    // т.к. там они уже будут переставлены в ConnectedClientCore.
    
    if ( _vcl_condition.agent_uuid == B738_AFF ) {
        
        _acf_condition.icao_type = "B738";
        _acf_condition.icao_airline = "AFF";
        _acf_condition.livery = "AFF";
        
    } else if ( _vcl_condition.agent_uuid == A321_AFL ) {
        
        _acf_condition.icao_type == "A321";
        _acf_condition.icao_airline = "AFL";
        _acf_condition.livery = "AFL";
        
    } else if ( _vcl_condition.agent_uuid == A321_SVR ) {
        
        _acf_condition.icao_type == "A321";
        _acf_condition.icao_airline = "SVR";
        _acf_condition.livery = "SVR";
        
    } else if ( _vcl_condition.agent_uuid == B772_UAE ) {
        
        _acf_condition.icao_type == "B772";
        _acf_condition.icao_airline = "UAE";
        _acf_condition.livery = "UAE";
        
    } else if ( _vcl_condition.agent_uuid == B744_SWI ) {
        
        _acf_condition.icao_type = "B744";
        _acf_condition.icao_airline = "SWI";
        _acf_condition.livery = "SWI";
        
    } else if ( _vcl_condition.agent_uuid == B763_SAS ) {
        
        _acf_condition.icao_type = "B763";
        _acf_condition.icao_airline = "SAS";
        _acf_condition.livery = "SAS";
        
    }

    CmdVehicleCondition::execute_on_server( client, server );
    
}
#endif
