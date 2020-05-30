// *********************************************************************************************************************
// *                                          Передача по сети состояния "самоходки"                                   *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 30 may 2020 at 14:44 *
// *********************************************************************************************************************

#include "cmd_vehicle_condition.h"
#include "utils.hpp"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                               Пустой конструктор                                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdVehicleCondition::CmdVehicleCondition() 
    : AbstractCommand()
{
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Конструктор от параметров                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

CmdVehicleCondition::CmdVehicleCondition(const vehicle_condition_t & vcl_condition) 
    : CmdVehicleCondition()
{
    _vcl_condition = vcl_condition;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из объекта в JSON                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void CmdVehicleCondition::to_json( JSON & json) {
    
    AbstractCommand::to_json( json );
    
    // Местоположение.
    json[ "latitude" ] = _vcl_condition.location.latitude;
    json[ "longitude" ] = _vcl_condition.location.longitude;
    json[ "altitude" ] = _vcl_condition.location.altitude;

    // Угловое положение. 
    json[ "heading" ] = _vcl_condition.rotation.heading;
    json[ "pitch" ] = _vcl_condition.rotation.pitch;
    json[ "roll" ] = _vcl_condition.rotation.roll;

    // Движение на плоскости.

    json[ "acceleration" ] = _vcl_condition.acceleration;
    // Скорость в узлах по сети не передается, 
    // ее можно посчитать из метров в секунду.
    json[ "speed" ] = _vcl_condition.speed;    
    json[ "target_speed" ] = _vcl_condition.target_speed;
    
    // Прижатость к земле.
    json[ "is_clamped_to_ground" ] = _vcl_condition.is_clamped_to_ground;

    // Курс (плоскостная величина).
    json[ "heading_acceleration" ] = _vcl_condition.heading_acceleration;
    json[ "target_heading" ] = _vcl_condition.target_heading;        

    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Преобразование из JSONа в объект                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void CmdVehicleCondition::from_json( JSON & json ) {
    
    AbstractCommand::from_json( json );
    
    // Местоположение.
    _vcl_condition.location.latitude = json.value( "latitude", 0.0 );
    _vcl_condition.location.longitude = json.value( "longitude", 0.0 );
    _vcl_condition.location.altitude = json.value( "altitude", 0.0 );
    
    // Угловое положение.
    _vcl_condition.rotation.heading = json.value( "heading", 0.0 );
    _vcl_condition.rotation.pitch = json.value( "pitch", 0.0 );
    _vcl_condition.rotation.roll = json.value( "roll", 0.0 );
    
    // Движение на плоскости.
    _vcl_condition.acceleration = json.value( "acceleration", 0.0 );
    _vcl_condition.speed = json.value( "speed", 0.0 );
    _vcl_condition.target_speed = json.value( "target_speed", 0.0 );
    
    // Прижатость к земле.
    _vcl_condition.is_clamped_to_ground = json.value( "is_clamped_to_ground", false );
    
    // Курс
    _vcl_condition.heading_acceleration = json.value( "heading_acceleration", 0.0 );
    _vcl_condition.target_heading = json.value( "target_heading", 0.0 );
    
    // То, что по сети не передается, считается на локальной стороне.
    _vcl_condition.speed_kts = xenon::meters_per_second_to_knots( _vcl_condition.speed );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                 Метод выполнения команды на сервере (в коммуникаторе)                             *
// *                                                                                                                   *
// *********************************************************************************************************************
#ifdef SERVER_SIDE
void CmdVehicleCondition::execute_on_server( ConnectedClientListener * client, ClientsListener * server ) {
};
#endif
