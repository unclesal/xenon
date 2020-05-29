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
, XPMP2::Aircraft(icao_type, icao_airline, livery )
#else
, ExternalAircraft( icao_type, icao_airline, livery )
#endif
{

#ifdef INSIDE_XPLANE

    bClampToGround = false;
    for (auto i=0; i<XPMP2::V_COUNT; i++ ) {
        v[i] = 0.0;
        __actuators[i] = actuator_motion_t();
    }
    // Времена полной отработки механизмов.
    __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = TIME_FOR_GEAR_MOTION;
    
#endif

    __graph = new AircraftStateGraph( this );
    __taxing_prepared = false;
    
    // Коррекция параметров для данного самолета.
    __acf_parameters_correction();

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Старт действия, предусмотренного следующей точкой полетного плана                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::__start_fp0_action() {
    
    if ( _flight_plan.empty() ) {
        XPlane::log("ERROR: BimboAircraft::__start_fp0_action called, but flight plan is empty");
    };
    
    // Проверки на индексы здесь не выполняется, т.к. она сделана
    // внутри процедуры установки текущего действия графа.
    
    auto next_wp = _flight_plan.at(0);    
    aircraft_state_graph::graph_t::edge_descriptor fake;
    try {
        aircraft_state_graph::graph_t::edge_descriptor action 
            = __graph->get_action_outgoing_from_current_state( next_wp.action_to_achieve );
            
        if ( action == fake ) {
            XPlane::log("ERROR: __start_fp0_action got fake edge descriptor");
            return;
        }
        __graph->set_active_action( action );

    } catch ( const std::range_error & re ) {
        XPlane::log(
            "ERROR: __start_fp0_action, invalid descriptor for action type " 
            + to_string( next_wp.action_to_achieve ) 
            + ", message=" + string( re.what() )
        );
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                            Выбор следующего действия                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::choose_next_action() {  
    
    string current_state_name = "Unknown state";
    auto current_state = __graph->get_current_state();
    if ( current_state ) {
        auto node = __graph->get_node_for( current_state );
        current_state_name = node.name;
    }
    XPlane::log("choose_next_action(), state=" + current_state_name + ", fp size=" + to_string( _flight_plan.size() ) );
    
    if (
        ( __graph->current_state_is( ACF_STATE_PARKING )) 
        && ( __graph->current_action_is( ACF_DOES_NOTHING ))
        && ( __taxing_prepared )
        && ( ! _flight_plan.empty() )
    ) {
        // На парковке и "ничего не делает", и при этом рулежка уже
        // подготовлена - поехали по полетному плану.
        __start_fp0_action();
        return;
    }
        
    if (
        ( __graph->current_state_is( ACF_STATE_READY_FOR_TAXING ) )
        && ( __taxing_prepared )
    ) {
        // Если готов к рулению - пока что поехали. 
        // TODO: всякая фигня типа заведения двигателей, разрешения на руление и др.
        __start_fp0_action();
        return;
    } 
    
    if ( __graph->current_state_is( ACF_STATE_HP ) ) {
        __start_fp0_action();
        return;
    }
    
    if ( __graph->current_state_is( ACF_STATE_READY_FOR_TAKE_OFF ) ) {
        __start_fp0_action();
        return;
    };
    
    if ( __graph->current_state_is( ACF_STATE_AIRBORNED ) ) {
        
        aircraft_state_graph::graph_t::edge_descriptor action 
            = __graph->get_action_outgoing_from_current_state( ACF_DOES_FLYING );
            
        __graph->set_active_action( action );
        
        return;
    };
    
    if ( __graph->current_state_is ( ACF_STATE_ON_FINAL ) ) {
        __start_fp0_action();
        return;
    }
    
    XPlane::log("ERROR: BimboAircraft::choose_next_action(), action was not determined");    
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                            Действие было завершено, переход в следующее состояние                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::_action_finished( void * action ) {
    
    AircraftAbstractAction * ptr_abstract_action = ( AircraftAbstractAction * ) action;
    aircraft_state_graph::edge_t edge = __graph->get_edge_for( ptr_abstract_action );
    XPlane::log("Action " + edge.name + " finished");
    __graph->action_finished( ptr_abstract_action );
    choose_next_action();
    
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
}
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
    set_rotation( rotation );
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
    set_location( ramp.location );
    SetHeading( ramp.heading );
    v[ V_CONTROLS_GEAR_RATIO ] = 1.0;
#endif

    // Сдвиг относительно начала стоянки
    move( shift_from_ramp() );    
    // В графе состояний отмечаем, что мы встали на стоянку.
    __graph->place_on_parking();

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
        _params.v1 = 100.0;
        _params.v2 = 120.0;
        _params.climb_speed = 240.0;
        _params.cruise_speed = 300.0;
        _params.descent_speed = 220.0;
        _params.landing_speed = 140.0;
        _params.vertical_climb_speed = 1900.0;
        _params.vertical_descend_speed = 1400.0;
        
        _params.take_off_angle = 8.0;
        _params.taxing_pitch = -1.8;
        _params.on_ground_offset = 3.25;
        
        _params.flaps_take_off_position = 0.35;
        _params.flaps_take_off_speed = 200.0;
        _params.flaps_landing_speed = 170.0;
        
        __actuators[ V_CONTROLS_FLAP_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_THRUST_RATIO ].full_time = 30.0;
        __actuators[ V_CONTROLS_THRUST_REVERS ].full_time = 5.0;
        __actuators[ V_CONTROLS_SPEED_BRAKE_RATIO ].full_time = 5.0;
        
    } else XPlane::log("BimboAircraft::__acf_parameters_correction(), not applied for " + acIcaoType );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Смещение в метрах относительно координат стоянки                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

float BimboAircraft::shift_from_ramp() {

    if ( acIcaoType == "B738" ) {
        return -9.0;
    }
    return -5.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Подготовка самолета к выруливанию на взлет и взлету                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::prepare_for_take_off( const deque<waypoint_t> & taxi_way ) {
    
    // Если самолет не на стоянке, то это ошибка.
    if ( ! __graph->current_state_is( ACF_STATE_PARKING ) ) {
        XPlane::log("ERROR: BimboAircraft::prepare_for_take_off, but aircraft is not parked.");
        return;
    }
        
    // Самая первая с конца точка - дальний конец ВПП.
    
    waypoint_t wp = taxi_way.at( taxi_way.size() - 1 );
    wp.action_to_achieve = ACF_DOES_TAKE_OFF;
    wp.location.altitude = 150.0;
    _flight_plan.push_front( wp );
        
    // Вторая точка - это ближний конец ВПП. На нее выходим из состояния HP
    // выравниванием ( lining up )
    wp = taxi_way.at( taxi_way.size() - 2 );
    wp.action_to_achieve = ACF_DOES_LINING_UP;
    _flight_plan.push_front( wp );

    // Дальше все точки - это рулежка.    
    for ( int i = (int) taxi_way.size() - 3; i>=0; -- i) {
        wp = taxi_way.at(i);
        wp.action_to_achieve = ACF_DOES_NORMAL_TAXING;
        _flight_plan.push_front( wp );
    }
    
    // До самой первой точки руления можно добраться либо выруливанием,
    // либо выталкиванием. Зависит от того, где она находится от нас,
    // спереди или сзади и можно ли до нее доехать самостоятельно.
    
    location_t current_location = get_location();
    double azimuth = xenon::bearing( current_location, _flight_plan.at(0).location );
    if ( ( azimuth <= 60.0 ) || ( azimuth >= 300.0 ) )
        // Это "выруливание", потому что точка у нас перед носом.
        _flight_plan.at( 0 ).action_to_achieve = ACF_DOES_SLOW_TAXING;
    else
        _flight_plan.at( 0 ).action_to_achieve = ACF_DOES_PUSH_BACK;
    
    __taxing_prepared = true;
            
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

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Управление движением самолета.                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::UpdatePosition(float elapsed_since_last_call, [[maybe_unused]] int fl_counter) {    
    __update_actuators(elapsed_since_last_call);
    __graph->update( elapsed_since_last_call );
#ifdef INSIDE_XPLANE
    if ( is_clamped_to_ground ) clamp_to_ground();
#endif
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Переместить самолет в 3D игровых координатах согласно его курсу                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::move( float meters ) {

#ifdef INSIDE_XPLANE

    auto radians = degrees_to_radians( drawInfo.heading );

    float dx = meters * sinf( radians );
    float dz = meters * cosf( radians );
    drawInfo.x += dx;
    drawInfo.z -= dz;

#else

    auto location = get_location();
    auto dest = xenon::shift( location, meters, GetHeading() );
    set_location( dest );

#endif

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Добавить точки в полетный план.                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::prepare_flight_plan( deque < waypoint_t > & fp, const float & cruise_altitude ) {
    
    // Прямо по входному массиву - ничего страшного.
    // Потому что выходной массив, т.е. действующий полетный план,
    // вообще-то уже может что-то содержать.
    
    for ( int i=0; i < (int) fp.size() - 1; ++i ) {
        waypoint_t & at_i = fp.at( i );
        waypoint_t & at_n = fp.at( i + 1 );
        at_i.distance_to_next_wp = xenon::distance2d(at_i.location, at_n.location );
        auto bearing = xenon::bearing( at_i.location, at_n.location );
        at_i.outgoing_heading = bearing;
        at_n.incomming_heading = bearing;
    };
    
    // TODO: вообще-то - не факт, что в самый конец, они могут и перетасовываться же? 
    for ( int i=0; i < (int) fp.size(); ++ i ) {
        _flight_plan.push_back( fp.at( i ) );
    };
    
    _params.cruise_altitude = cruise_altitude;
    
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
    
    auto wp = _flight_plan.at(0);
    while ( 
        ( wp.type != WAYPOINT_RUNWAY ) 
        && ( wp.action_to_achieve != ACF_DOES_LINING_UP ) 
        && ( ! _flight_plan.empty()) 
    ) {
        _flight_plan.pop_front();
        if ( ! _flight_plan.empty() ) wp = _flight_plan.at( 0 ); 
    }
    
    if ( wp.action_to_achieve != ACF_DOES_LINING_UP ) {
        XPlane::log("ERROR: impossible starting from HP due FP content");
        return;
    };
    
    auto on_hp = __graph->get_node_for( ACF_STATE_HP );
    __graph->set_active_state( on_hp );
    
    __start_fp0_action();
    
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
    for ( i=0; i<_flight_plan.size(); i++ ) {
        auto wp = _flight_plan.at(i);
        if ( 
            ( wp.type == WAYPOINT_RUNWAY ) 
            && ( wp.action_to_achieve == ACF_DOES_TAKE_OFF ) 
        ) {
            XPlane::log("Found WP at index " + to_string(i));
            break;
        }
    }
    
    if ( i >= _flight_plan.size() ) {
        XPlane::log("ERROR: end point of RWY can not does not exists in FP");
        return;
    };
    
    for ( int k=0; k<i; k++ ) _flight_plan.pop_front();    
    auto wp = _flight_plan.front();
    if ( ( wp.type != WAYPOINT_RUNWAY ) || ( wp.action_to_achieve != ACF_DOES_TAKE_OFF ) ) {
        XPlane::log("ERROR: waypoint for end runway was not found");
        return;
    }
    
    auto position = XPlane::location_to_position( wp.location );
    position.y = 170.0 + 200.0;
    rotation_t rotation;
    rotation.heading = wp.incomming_heading;
    is_clamped_to_ground = true;
    place_on_ground( position, rotation, false );    
    
}
#endif

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Тестирование полетной фазы                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

#ifdef INSIDE_XPLANE
void BimboAircraft::test__fly() {
    
    deque< waypoint_t > fp;
    
//     При вылете с полосы 08L
//     SS028
//     waypoint_t ss028 = {
//         .name = "SS028",
//         .type = WAYPOINT_FLYING,
//         .location = {
//             .latitude = degrees_to_decimal( 56, 44, 40.20, 'N' ),
//             .longitude = degrees_to_decimal( 60, 59, 28.50, 'E' ),
//             .altitude = 700.0
//         },
//         .speed = 240.0,
//         .incomming_heading = 0.0,
//         .outgoing_heading = 0.0,        
//         .distance_to_next_wp = 0.0,
//         .action_to_achieve = ACF_DOES_FLYING
//     };
//     fp.push_back( ss028 );
    
//     D237K
//     waypoint_t d237k = {
//         .name = "D237K",
//         .type = WAYPOINT_FLYING,
//         .location = {
//             .latitude = degrees_to_decimal( 56, 41, 12.40, 'N' ),
//             .longitude = degrees_to_decimal( 60, 29, 1.46, 'E' ),
//             .altitude = 700.0
//         },
//         .speed = 240.0,
//         .incomming_heading = 0.0,
//         .outgoing_heading = 0.0,        
//         .distance_to_next_wp = 0.0,
//         .action_to_achieve = ACF_DOES_FLYING
//     };
//     fp.push_back( d237k );
//     
     
    // SS025 
    waypoint_t ss025 = {
        .name = "SS025",
        .type = WAYPOINT_FLYING,
        .location = {
            .latitude = degrees_to_decimal( 56, 44, 42.11, 'N' ),
            .longitude = degrees_to_decimal( 60, 28, 31.40, 'E' ),
            .altitude = 900.0
        },
        .speed = 240.0,
        .incomming_heading = 0.0,
        .outgoing_heading = 0.0,        
        .distance_to_next_wp = 0.0,
        .action_to_achieve = ACF_DOES_FLYING
    };
    fp.push_back( ss025 );
    
    // CF08L
    waypoint_t cf08l = {
        .name = "CF08L",
        .type = WAYPOINT_FLYING,
        .location = {
            .latitude = degrees_to_decimal( 56, 44, 41.97, 'N' ),
            .longitude = degrees_to_decimal( 60, 34, 0.50, 'E' ),
            .altitude = 900.0
        },
        .speed = 190.0,
        .incomming_heading = 0.0,
        .outgoing_heading = 0.0,        
        .distance_to_next_wp = 0.0,
        .action_to_achieve = ACF_DOES_FLYING
    };
    fp.push_back( cf08l );
    
    // RW08L 
    waypoint_t rwy08l = {
        .name = "RWY08L",
        .type = WAYPOINT_RUNWAY,
        .location = {
            .latitude = degrees_to_decimal( 56, 44, 41.41, 'N' ),
            .longitude = degrees_to_decimal( 60, 46, 40.90, 'E' ),
            .altitude = 170.0
        },
        .speed = _params.landing_speed,
        .incomming_heading = 0.0,
        .outgoing_heading = 0.0,          
        .distance_to_next_wp = 0.0,
        .action_to_achieve = ACF_DOES_LANDING
    };
    fp.push_back( rwy08l );
    
    // RWY26R - только для обеспечения посадки, для установки курса и торможения на ВПП.
    waypoint_t rwy26r = {
        .name = "RWY26R",
        .type = WAYPOINT_DESTINATION,
        .location = {
            .latitude = degrees_to_decimal( 56, 44, 40.99, 'N' ),
            .longitude = degrees_to_decimal( 60, 49, 22.90, 'E' ),
            .altitude = 600.0
        },
        .speed = 0.0,
        .incomming_heading = 0.0,        
        .outgoing_heading = 0.0,        
        .distance_to_next_wp = 0.0,
        .action_to_achieve = ACF_DOES_LANDING
    };
    fp.push_back( rwy26r );
     
    prepare_flight_plan( fp, 1000.0f );
        
    // Для теста встаем на последнюю точку ВПП по горизонтали,
    // но - в небе, типа только что взлетели.
    is_clamped_to_ground = false;
    
//     location_t start_point = {
//         .latitude = degrees_to_decimal( 56, 44, 40.99, 'N' ),
//         .longitude = degrees_to_decimal( 60, 49, 22.90, 'E' ),
//         .altitude = 600.0
//     };
//     set_location( start_point );
    set_location( fp.at(0).location );
    
    
    auto rotation = get_rotation();
    rotation.heading = 70.0;
    rotation.pitch = 1.0;
    set_rotation( rotation );
    
    v[ V_CONTROLS_GEAR_RATIO ] = 0.0;
    v[ V_CONTROLS_SPEED_BRAKE_RATIO ] = 0.0;
        
    // Состояние AIRBORNED имеет только одно исходящее действие, это полет.
    __graph->set_active_state( ACF_STATE_AIRBORNED );    
    
}
#endif
