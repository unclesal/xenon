// *********************************************************************************************************************
// *                                        "Ботовый" (multiplayer) самолет.                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 01 may 2020 at 12:11 *
// *********************************************************************************************************************
#include <math.h>

#ifdef INSIDE_XPLANE
#include "XPLMScenery.h"
#endif

#include "bimbo_aircraft.h"
#include "constants.h"
#include "utils.hpp"
#include "structures.h"

using namespace xenon;

#ifdef INSIDE_XPLANE
using namespace XPMP2;
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Конструктор                                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

BimboAircraft::BimboAircraft(
    const std::string & icao_type,
    const std::string & icao_airline,
    const std::string & livery
)
    : AbstractAircraft()
#ifdef INSIDE_XPLANE
    // Находимся - внутри X-Plane, наследуемся от XPMP2::Aircraft
    , XPMP2::Aircraft(icao_type, icao_airline, livery )
#else
    // Находимся - снаружи X-Plane.
    , acIcaoType( icao_type )    
    , acIcaoAirline( icao_airline )
    , acLivery( livery )
#endif
{

#ifdef INSIDE_XPLANE

    // Внутри X-Plane, в плагине.
    
    bClampToGround = false;
    for (auto i=0; i<XPMP2::V_COUNT; i++ ) {
        v[i] = 0.0;
        __actuators[i] = actuator_motion_t();
    }
    
#else

    // Снаружи X-Plane, в агенте.
    acf_condition.icao_type = icao_type;
    acf_condition.icao_airline = icao_airline;
    acf_condition.livery = livery;
    
#endif

    graph = new AircraftStateGraph( this );
    __taxing_prepared = false;
    
    // Коррекция параметров для данного самолета.
    __acf_parameters_correction();

}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                            Действие было завершено, переход в следующее состояние                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::action_finished( void * action ) {
    
    AircraftAbstractAction * ptr_abstract_action = reinterpret_cast< AircraftAbstractAction * >( action );
#ifdef INSIDE_AGENT        
    // aircraft_state_graph::edge_t edge = graph->get_edge_for( ptr_abstract_action );
    graph->action_finished( ptr_abstract_action );    
    AbstractVehicle::action_finished( action );
#endif        
    
#ifdef INSIDE_XPLANE    
    graph->set_active_action( ACF_DOES_NOTHING );
#endif
    
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             Перекрытая функция касания земли с учетом высоты самолета.                            *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void BimboAircraft::hit_to_ground( position_t & position ) {
    
    AbstractVehicle::hit_to_ground( position );
    // Учет высоты данной модели воздушного судна.
    position.y += _params.on_ground_offset;

}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Перекрытая процедура наблюдения за объектом                                     *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void BimboAircraft::observe() {

}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Перекрытая процедура управления (просчета положения)                               *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void BimboAircraft::control( float elapsed_since_last_call ) {

}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Перекрытый метод получения внутриигровой позиции.                                   *
// *                                                                                                                   *
// *********************************************************************************************************************
#ifdef INSIDE_XPLANE
position_t BimboAircraft::get_position() {
    position_t position;
    position.x = drawInfo.x;
    position.y = drawInfo.y;
    position.z = drawInfo.z;
    return position;
}
#endif
// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Установка внутриигровой позиции инстанции - от структуры position_t.                       *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void BimboAircraft::set_position( const position_t & position ) {
    
    drawInfo.x = position.x;
    drawInfo.y = position.y;
    drawInfo.z = position.z;
    
    auto location = XPlane::position_to_location( position );
    vcl_condition.location = location;
    
}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Установка локации - внутри X-Plane                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void BimboAircraft::set_location( const location_t & location ) {    
    // Установка vcl_condition проходит там же.
    AbstractAircraft::set_location( location );    
};
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Установка уголового положения самолета                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::set_rotation( const rotation_t & rotation ) {

    SetPitch( rotation.pitch );
    SetHeading( rotation.heading );
    SetRoll( rotation.roll );
    
#ifdef INSIDE_XPLANE    
    vcl_condition.rotation = rotation;
#endif
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Расположить самолет на земле в произвольном месте с прозивольными углами (рулежка)                 *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void BimboAircraft::place_on_ground( const position_t & position, rotation_t & rotation, bool clamp ) {    
    set_position( position );
    rotation.pitch = _params.taxing_pitch;
    
    // Для проверки величины установленного взлетного угла.
    // rotation.pitch = _params.take_off_angle;
    
    set_rotation( rotation );
    vcl_condition.is_clamped_to_ground = true;
    // Если самолет на земле, то шасси-то у него точно выпущены же.
    v[ V_CONTROLS_GEAR_RATIO ] = 1.0;
}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Расположить самолет на стоянке.                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::place_on_ground( const startup_location_t & ramp ) {

#ifdef INSIDE_XPLANE
    // Переводим геолокацию в OGL игровые координаты.
    position_t position = XPlane::location_to_position( ramp.location );
    // Угловое положение самолета.
    rotation_t rotation;
    rotation.heading = ramp.heading;    
    // Первоначальная, грубая установка позиции.
    place_on_ground(position, rotation);
#else
    vcl_condition.is_clamped_to_ground = true;
    set_location( ramp.location );
    SetHeading( ramp.heading );        
#endif

    // Здесь уже все равно, внутри X-Plane или нет.
    set_gear_down( true);

    // Сдвиг относительно начала стоянки
    move( _params.shift_from_ramp );
    // В графе состояний отмечаем, что мы встали на стоянку.
    graph->place_on_parking();
    
    acf_condition.parking = ramp.name;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Вернуть угловое положение самолета                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

rotation_t BimboAircraft::get_rotation() {        
    // У "имитационного" самолета нет разницы между
    // истинными и магнитным курсом, выдаем один к одному.
    rotation_t rotation;
    rotation.pitch = GetPitch();
    rotation.heading = GetHeading();
    rotation.roll = GetRoll();
    return rotation;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                 Коррекция параметров в зависимости от типа самолета                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::__acf_parameters_correction() {
    
    if ( acIcaoType == "B738" ) {
        _params.length = 40.0;
        _params.rotation_center = _params.length * 2.0f / 3.5f;
        _params.wingspan = 34.0;
        _params.shift_from_ramp = -9.0;

        _params.v1 = 100.0;
        _params.v2 = 120.0;
        _params.climb_speed = 240.0;
        _params.cruise_speed = 300.0;
        _params.descent_speed = 220.0;
        _params.landing_speed = 140.0;
        _params.vertical_climb_speed = 1900.0;
        _params.vertical_descend_speed = 1400.0;
        
        _params.take_off_angle = 8.0;
        _params.taxing_pitch = -1.85;
        _params.on_ground_offset = 3.25;
        
        _params.flaps_take_off_position = 0.35;
        _params.flaps_take_off_speed = 200.0;
        _params.flaps_landing_speed = 170.0;

#ifdef INSIDE_XPLANE        
        __actuators[ V_CONTROLS_FLAP_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_THRUST_RATIO ].full_time = 30.0;
        __actuators[ V_CONTROLS_THRUST_REVERS ].full_time = 5.0;
        __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].full_time = 5.0;
#endif
        
    } else if ( acIcaoType == "A321" ) {
        
        _params.length = 45.0;
        _params.rotation_center = _params.length * 2.0f / 3.0f;
        _params.wingspan = 34.0;
        _params.shift_from_ramp = -9.0;

        _params.v1 = 100.0;
        _params.v2 = 120.0;
        _params.climb_speed = 240.0;
        _params.cruise_speed = 300.0;
        _params.descent_speed = 220.0;
        _params.landing_speed = 140.0;
        _params.vertical_climb_speed = 1900.0;
        _params.vertical_descend_speed = 1400.0;
        
        _params.take_off_angle = 8.0;
        _params.taxing_pitch = -1.35;
        _params.on_ground_offset = 3.5;
        
        _params.flaps_take_off_position = 0.35;
        _params.flaps_take_off_speed = 200.0;
        _params.flaps_landing_speed = 170.0;

#ifdef INSIDE_XPLANE        
        __actuators[ V_CONTROLS_FLAP_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_THRUST_RATIO ].full_time = 30.0;
        __actuators[ V_CONTROLS_THRUST_REVERS ].full_time = 5.0;
        __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].full_time = 5.0;
#endif

    } else if ( acIcaoType == "B744" ) {

        _params.length = 70.6;
        _params.rotation_center = _params.length / 4.0f;
        _params.wingspan = 64.4;
        _params.shift_from_ramp = -20.0;

        _params.v1 = 120.0;
        _params.v2 = 150.0;
        _params.climb_speed = 240.0;
        _params.cruise_speed = 300.0;
        _params.descent_speed = 220.0;
        _params.landing_speed = 160.0;
        _params.vertical_climb_speed = 1900.0;
        _params.vertical_descend_speed = 1400.0;
        
        _params.take_off_angle = 9.0;
        _params.taxing_pitch = -0.2;
        _params.on_ground_offset = 5.2;
        
        _params.flaps_take_off_position = 0.4;
        _params.flaps_take_off_speed = 210.0;
        _params.flaps_landing_speed = 180.0;

#ifdef INSIDE_XPLANE        
        __actuators[ V_CONTROLS_FLAP_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_THRUST_RATIO ].full_time = 30.0;
        __actuators[ V_CONTROLS_THRUST_REVERS ].full_time = 5.0;
        __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].full_time = 5.0;
#endif
        
    } else if ( acIcaoType == "B763" ) {
        
        _params.length = 54.94;
        _params.rotation_center = _params.length / 2.5f;
        _params.wingspan = 47.57;
        _params.shift_from_ramp = -15.0;

        _params.v1 = 110.0;
        _params.v2 = 140.0;
        _params.climb_speed = 240.0;
        _params.cruise_speed = 320.0;
        _params.descent_speed = 220.0;
        _params.landing_speed = 150.0;
        _params.vertical_climb_speed = 1900.0;
        _params.vertical_descend_speed = 1400.0;
        
        _params.take_off_angle = 10.0;
        _params.taxing_pitch = -1.0;
        _params.on_ground_offset = 4.5;
        
        _params.flaps_take_off_position = 0.35;
        _params.flaps_take_off_speed = 200.0;
        _params.flaps_landing_speed = 170.0;

#ifdef INSIDE_XPLANE        
        __actuators[ V_CONTROLS_FLAP_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_THRUST_RATIO ].full_time = 30.0;
        __actuators[ V_CONTROLS_THRUST_REVERS ].full_time = 5.0;
        __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].full_time = 5.0;
#endif
        
    } else if ( acIcaoType == "B772" ) {
        
        _params.length = 63.73;
        _params.rotation_center = _params.length / 2.2f;
        _params.wingspan = 60.93;
        _params.shift_from_ramp = -25.0;

        _params.v1 = 110.0;
        _params.v2 = 140.0;
        _params.climb_speed = 240.0;
        _params.cruise_speed = 320.0;
        _params.descent_speed = 220.0;
        _params.landing_speed = 150.0;
        _params.vertical_climb_speed = 1900.0;
        _params.vertical_descend_speed = 1400.0;
        
        _params.take_off_angle = 11.5;
        _params.taxing_pitch = -1.7;
        _params.on_ground_offset = 5.7;
        
        _params.flaps_take_off_position = 0.35;
        _params.flaps_take_off_speed = 200.0;
        _params.flaps_landing_speed = 170.0;

#ifdef INSIDE_XPLANE        
        __actuators[ V_CONTROLS_FLAP_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_THRUST_RATIO ].full_time = 30.0;
        __actuators[ V_CONTROLS_THRUST_REVERS ].full_time = 5.0;
        __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].full_time = 5.0;
#endif

    } else Logger::log("BimboAircraft::__acf_parameters_correction(), not applied for " + acIcaoType );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Подготовка самолета к выруливанию на взлет и взлету                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::prepare_for_take_off( const deque<waypoint_t> & taxi_way ) {
    
    // Если самолет не на стоянке, то это ошибка.
    if ( ! graph->current_state_is( ACF_STATE_PARKING ) ) {
        Logger::log("ERROR: BimboAircraft::prepare_for_take_off, but aircraft is not parked.");
        return;
    }
    
    waypoint_t wp;
    
    for ( int i=taxi_way.size() - 1; i>=0; --i ) {
        wp = taxi_way.at( i );
        flight_plan.push_front( wp );
    };
            
    // До самой первой точки руления можно добраться либо выруливанием,
    // либо выталкиванием. Зависит от того, где она находится от нас,
    // спереди или сзади и можно ли до нее доехать самостоятельно.
    
    location_t current_location = get_location();
    double azimuth = xenon::bearing( current_location, flight_plan.get(0).location );
    if ( ( azimuth <= 60.0 ) || ( azimuth >= 300.0 ) ) {
        // Это "выруливание", потому что точка у нас перед носом.
        wp = flight_plan.get( 0 );
        wp.action_to_achieve = ACF_DOES_SLOW_TAXING;
        flight_plan.set(0, wp );
    } else {
        wp = flight_plan.get( 0 );
        wp.action_to_achieve = ACF_DOES_PUSH_BACK;
        flight_plan.set( 0, wp );
    }
    
    __taxing_prepared = true;
            
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                          Подготовка самолета к рулению без дополнительных обязательств                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::prepare_for_taxing( const deque < xenon::waypoint_t > & taxi_way) {
    
    for ( auto wp: taxi_way ) {
        flight_plan.push_back( wp );
    }
    
    // Если последняя точка парковка - то ее нужно сместить в зависимости от типа ВС.
    auto wpp = flight_plan.get( flight_plan.size() - 1 );
    if ( wpp.type == WAYPOINT_PARKING ) {
        auto dest = xenon::shift( wpp.location, _params.shift_from_ramp, wpp.incomming_heading );
        wpp.location = dest;
        flight_plan.set( flight_plan.size() - 1, wpp );
    }
    
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Вернуть позицию самолета в игровом пространстве                                     *
// *                                                                                                                   *
// *********************************************************************************************************************
#ifdef INSIDE_XPLANE
position_with_angles_t BimboAircraft::get_position_with_angles() {
    position_with_angles_t pos;
    pos.position = get_position();
    pos.rotation = get_rotation();
    return pos;
}
#endif
// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Изменение положения актуаторов (управляющих поверхностей)                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void BimboAircraft::__update_actuators( float elapsed_since_last_call ) { // NOLINT(bugprone-reserved-identifier)
    for ( auto i=0; i<V_COUNT; i++ ) {
        if ( __actuators[i].requested ) {
            
            float current_value = v[i];
            if (current_value != __actuators[i].endpoint) {
                
                float delta = elapsed_since_last_call / __actuators[i].full_time;                                                
                if ( __actuators[i].endpoint < current_value ) delta = -delta;
                
                v[i] += delta;                
                if ( delta < 0 ) {
                    
                    // Отрицательное - может уйти меньше заданного.
                    if ( v[i] <= __actuators[i].endpoint ) {
                        // Достигли конечной точки.
                        v[i] = __actuators[i].endpoint;
                        __actuators[i].requested = false;
                    };
                    
                } else {
                    
                    // Положительное - может уйти выше заданного.
                    if ( v[i] >= __actuators[i].endpoint ) {
                        // Достигли конечной точки.
                        v[i] = __actuators[i].endpoint;
                        __actuators[i].requested = false;
                    }

                };
                
                
            } else __actuators[i].requested = false;

        }
    }
};
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Управление движением самолета.                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::UpdatePosition(float elapsed_since_last_call, [[maybe_unused]] int fl_counter) {    
    
    _acf_mutex.lock();
    
    graph->update( elapsed_since_last_call );

#ifdef INSIDE_XPLANE
    
    __update_actuators(elapsed_since_last_call);
    if ( vcl_condition.is_clamped_to_ground ) {
        clamp_to_ground();
    }        
    
#endif
    _acf_mutex.unlock();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Переместить самолет в 3D игровых координатах согласно его курсу                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::move( float meters ) {

    
#ifdef INSIDE_XPLANE

    // Внутри X-Plane - вариант, при котором существенно меньше вычислений.
    
    auto position = get_position();
    auto dest = XPlane::shift( position, meters, GetHeading() );
    drawInfo.x = dest.x;
    drawInfo.z = dest.z;
    
    auto location = get_location();
    vcl_condition.location = location;

#else
    
    // Снаружи X-Plane - вычисления через геоид, 
    // т.к. "игровых координат" здесь нет.
    
    auto location = get_location();
    auto dest = xenon::shift( location, meters, GetHeading() );
    set_location( dest );

#endif

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                          Изменение состояния самолета от пришедшей по сети структуры                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::update_from( vehicle_condition_t & vc, aircraft_condition_t & ac ) {
        
    _acf_mutex.lock();
        
#ifdef INSIDE_XPLANE
    // Существующая прижатость к земле - возможно, понадобится ее 
    // восстановить как была в X-Plane. Чтобы не дергался по высоте.
    auto is_clamped_to_ground = vcl_condition.is_clamped_to_ground;
    auto altitude = drawInfo.y;
    auto pitch = drawInfo.pitch;
#endif
    
    AbstractAircraft::update_from( vc, ac );
            
    if ( ! graph->current_state_is( vc.current_state ))
            graph->set_active_state( vc.current_state );
    
    if ( ! graph->current_action_is( vc.current_action ) ) 
            graph->set_active_action( vc.current_action );        
        
#ifdef INSIDE_XPLANE
    
    
    auto distance = xenon::distance2d(get_location(), vc.location );
    if ( distance >= 1.0 ) {
        set_location( vc.location );
    };
    
    float rotation_threshold = 1.0f;
    auto rotation = get_rotation();
    if (
        ( abs( rotation.heading - vc.rotation.heading ) >= rotation_threshold )
        || ( abs( rotation.pitch - vc.rotation.pitch ) >= rotation_threshold )
        || ( abs( rotation.roll - vc.rotation.roll ) >= rotation_threshold )
    ) {
        set_rotation( vc.rotation );
    }
    
    if ( 
            graph->current_action_is( ACF_DOES_LANDING )
            || graph->current_action_is( ACF_DOES_TAKE_OFF )
            || graph->current_state_is ( ACF_STATE_ON_FINAL )       
    ) {
        
        vcl_condition.is_clamped_to_ground = is_clamped_to_ground;
        
        drawInfo.y = altitude;
        vcl_condition.location.altitude = altitude;
        
        drawInfo.pitch = pitch;
        vcl_condition.rotation.pitch = pitch;
        
    }
#else
    set_location( vc.location );
    set_rotation( vc.rotation );
#endif    
    _acf_mutex.unlock();
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                С целью тестирования, чтобы не ждать слишком долго - расположить на предварительном                *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void BimboAircraft::test__place_on_hp() {
    location_t location;
    rotation_t rotation;
    
    // 08L
    location.latitude = 56.746317;
    location.longitude = 60.780804;
    rotation.heading = 180.161499;
    
    position_t position = XPlane::location_to_position( location );    
    
    
    place_on_ground( position, rotation, true );
    
    auto wp = flight_plan.get(0);
    while ( 
        ( wp.type != WAYPOINT_RUNWAY ) 
        && ( wp.action_to_achieve != ACF_DOES_LINING_UP ) 
        && ( ! flight_plan.is_empty()) 
    ) {
        flight_plan.pop_front();
        if ( ! flight_plan.is_empty() ) wp = flight_plan.get( 0 ); 
    }
    
    if ( wp.action_to_achieve != ACF_DOES_LINING_UP ) {
        Logger::log("ERROR: impossible starting from HP due FP content");
        return;
    };
    
    auto on_hp = graph->get_node_for( ACF_STATE_HP );
    graph->set_active_state( on_hp );    
    
}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *        С целью тестирования - расположить самолет в конечной точке ВПП, где он должен быть уже взлетевшим         *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void BimboAircraft::test__place_on_rwy_end() {
    
    int i = 0;
    for ( i=0; i<flight_plan.size(); i++ ) {
        auto wp = flight_plan.get(i);
        if ( 
            ( wp.type == WAYPOINT_RUNWAY ) 
            && ( wp.action_to_achieve == ACF_DOES_TAKE_OFF ) 
        ) {
            Logger::log("Found WP at index " + to_string(i));
            break;
        }
    }
    
    if ( i >= flight_plan.size() ) {
        Logger::log("ERROR: end point of RWY can not does not exists in FP");
        return;
    };
    
    for ( int k=0; k<i; k++ ) flight_plan.pop_front();
    auto wp = flight_plan.get(0);
    if ( ( wp.type != WAYPOINT_RUNWAY ) || ( wp.action_to_achieve != ACF_DOES_TAKE_OFF ) ) {
        Logger::log("ERROR: waypoint for end runway was not found");
        return;
    }
    
    auto position = XPlane::location_to_position( wp.location );
    position.y = 170.0 + 200.0;
    rotation_t rotation;
    rotation.heading = wp.incomming_heading;
    vcl_condition.is_clamped_to_ground = true;
    place_on_ground( position, rotation, false );    
    
}
#endif

// *********************************************************************************************************************
// * *
// * Тестирование посадочной фазы (только полетный план) *
// * *
// *********************************************************************************************************************

void BimboAircraft::test__fp_landing() {
    
    // Отсюда начинается этап отладки посадки.
    // SS025
    waypoint_t ss025;
    ss025.name = "SS025";
    ss025.type = WAYPOINT_FLYING;
    ss025.location.latitude = degrees_to_decimal( 56, 44, 42.11, 'N' );
    ss025.location.longitude = degrees_to_decimal( 60, 28, 31.40, 'E' );
    ss025.location.altitude = 1100.0;        
    ss025.speed = 240.0;        
    ss025.action_to_achieve = ACF_DOES_FLYING;    
    flight_plan.push_back( ss025 );

    // CF08L
    waypoint_t cf08l;
    cf08l.name = "CF08L";
    cf08l.type = WAYPOINT_FLYING;
    cf08l.location.latitude = degrees_to_decimal( 56, 44, 41.97, 'N' );
    cf08l.location.longitude = degrees_to_decimal( 60, 34, 0.50, 'E' );
    cf08l.location.altitude = 1100.0;        
    cf08l.speed = 240.0;        
    cf08l.action_to_achieve = ACF_DOES_FLYING;
    flight_plan.push_back( cf08l );

    // FN08L
    waypoint_t fn08l;
    fn08l.name = "FN08L";
    fn08l.type = WAYPOINT_FLYING;
    fn08l.location.latitude = degrees_to_decimal(56, 44, 40.25, 'N' );
    fn08l.location.longitude = degrees_to_decimal(60, 37, 38.34, 'E');
    fn08l.location.altitude = 1000.0;        
    fn08l.speed = 200.0;        
    fn08l.action_to_achieve = ACF_DOES_GLIDING;
    flight_plan.push_back( fn08l );

    // RW08L - торец ВПП.    
    waypoint_t rwy08l;
    rwy08l.name = "RWY08L";
    rwy08l.type = WAYPOINT_RUNWAY;
    rwy08l.location.latitude = degrees_to_decimal( 56, 44, 41.41, 'N' );
    rwy08l.location.longitude = degrees_to_decimal( 60, 46, 40.90, 'E' );
    rwy08l.location.altitude = 260.0;        
    rwy08l.speed = _params.landing_speed;        
    rwy08l.action_to_achieve = ACF_DOES_LANDING;
    flight_plan.push_back( rwy08l );

    // RWY26R - только для обеспечения посадки, для установки курса и торможения на ВПП.
    waypoint_t rwy26r;
    rwy26r.name = "RWY26R";
    rwy26r.type = WAYPOINT_DESTINATION;
    rwy26r.location.latitude = degrees_to_decimal( 56, 44, 40.99, 'N' );
    rwy26r.location.longitude = degrees_to_decimal( 60, 49, 22.90, 'E' );
    rwy26r.location.altitude = 260.0;
    rwy26r.speed = 0.0;        
    rwy26r.action_to_achieve = ACF_DOES_LANDING;
    flight_plan.push_back( rwy26r );

};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Тестирование полетной фазы                                              *
// *                                                                                                                   *
// *********************************************************************************************************************


void BimboAircraft::test__fly() {
        
    // При вылете с полосы 08L
    // SS028
    waypoint_t ss028;
    ss028.name = "SS028";
    ss028.type = WAYPOINT_FLYING;
    ss028.location.latitude = degrees_to_decimal( 56, 44, 40.20, 'N' );
    ss028.location.longitude = degrees_to_decimal( 60, 59, 28.50, 'E' );
    ss028.location.altitude = 1100.0;    
    ss028.speed = 240.0;    
    ss028.action_to_achieve = ACF_DOES_BECOMING;
    flight_plan.push_back( ss028 );    
    
    // D237K
    waypoint_t d237k;
    d237k.name = "D237K";
    d237k.type = WAYPOINT_FLYING;
    d237k.location.latitude = degrees_to_decimal( 56, 41, 12.40, 'N' );
    d237k.location.longitude = degrees_to_decimal( 60, 29, 1.46, 'E' );
    d237k.location.altitude = 1100.0;       
    d237k.speed = 240.0;       
    d237k.action_to_achieve = ACF_DOES_FLYING;   
    flight_plan.push_back( d237k );
   
    test__fp_landing();
          
    vector< std::string > alternate;
    alternate.push_back("USCC");
    flight_plan.set_alternate( alternate );
    flight_plan.set_flight_number("TEST1");
    flight_plan.set_departure("USSS");
    flight_plan.set_destination("USSS");
    flight_plan.set_cruise_altitude( 5000.0f );
        
//     vcl_condition.is_clamped_to_ground = false;
// 
//     -------------- Какие-то похожие на правду скорости ------------------
//     vcl_condition.acceleration = 2.0;
//     vcl_condition.target_speed = 102.889; // 200 kph
//     vcl_condition.speed = 100.0;
// 
//     acf_condition.vertical_acceleration = 0.6f;
//     acf_condition.target_vertical_speed = feet_per_min_to_meters_per_second( _params.vertical_climb_speed );
//     ---------------------------------------------------------------------
// 
//    Для теста встаем на последнюю точку ВПП по горизонтали,
//    но - в небе, типа только что взлетели.
//     
//    location_t start_point = {
//        .latitude = degrees_to_decimal( 56, 44, 40.99, 'N' ),
//        .longitude = degrees_to_decimal( 60, 49, 22.90, 'E' ),
//        .altitude = 600.0
//    };
//    set_location( start_point );
//     
//     Это если хочется на первую точку полетного плана.
//     set_location( fp.at(0).location );
//    
//     auto rotation = get_rotation();
//     rotation.heading = 80.0;
//     rotation.pitch = 1.0;
//     set_rotation( rotation );
//     
//    v[ V_CONTROLS_GEAR_RATIO ] = 0.0;
//    v[ V_CONTROLS_SPEED_BRAKE_RATIO ] = 0.0;
// 
//     Состояние AIRBORNED имеет только одно исходящее действие, это полет.
//     graph->set_active_state( ACF_STATE_AIRBORNED );
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Тестирование руления на стоянку                                           *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void BimboAircraft::test__taxing() {

    auto usss = Airport::get_by_icao("USSS");

    location_t start_point = {
        .latitude = 56.744801,
        .longitude = 60.803618,
        .altitude = 170.0
    };
#ifdef INSIDE_XPLANE

    AbstractVehicle::hit_to_ground( start_point );
    vcl_condition.is_clamped_to_ground = true;
    v[ V_CONTROLS_GEAR_RATIO ] = 1.0;

#endif
    set_location( start_point );

    auto heading = 80.0;
    auto rotation = get_rotation();
    rotation.heading = heading;
    set_rotation( rotation );

    _params.destination = "USSS";
    acf_condition.icao_type = "B738";

    graph->set_active_state( ACF_STATE_LANDED );

}
*/
