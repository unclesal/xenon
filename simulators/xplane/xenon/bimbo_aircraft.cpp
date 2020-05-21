// *********************************************************************************************************************
// *                                        "Ботовый" (multiplayer) самолет.                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 01 may 2020 at 12:11 *
// *********************************************************************************************************************
#include <math.h>

#include "XPLMScenery.h"
#include "bimbo_aircraft.h"
#include "constants.h"
#include "utils.hpp"
#include "structures.h"

using namespace xenon;
using namespace XPMP2;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                 Конструктор                                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

BimboAircraft::BimboAircraft(
    const std::string & icao_type, const std::string & icao_airline,
    const std::string & livery
)
    : AbstractAircraft(), XPMP2::Aircraft(icao_type, icao_airline, livery )
{
    // _current_condition.speed = -0.4;
    // _current_condition.heading_shift = -0.5;

    bClampToGround = false;
    for (auto i=0; i<XPMP2::V_COUNT; i++ ) {
        v[i] = 0.0;
        __actuators[i] = actuator_motion_t();
    }
    // Времена полной отработки механизмов.
    __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = TIME_FOR_GEAR_MOTION;
    
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

position_t BimboAircraft::get_position() {
    position_t position;
    position.x = drawInfo.x;
    position.y = drawInfo.y;
    position.z = drawInfo.z;
    return position;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Установка внутриигровой позиции инстанции - от структуры position_t.                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::set_position( const position_t & position ) {
    drawInfo.x = position.x;
    drawInfo.y = position.y;
    drawInfo.z = position.z;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Установка уголового положения самолета                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::set_rotation( const rotation_t & rotation ) {
    drawInfo.pitch = rotation.pitch;
    drawInfo.heading = rotation.heading;
    drawInfo.roll = rotation.roll;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Расположить самолет на земле в произвольном месте с прозивольными углами (рулежка)                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::place_on_ground( const position_t & position, const rotation_t & rotation, bool clamp ) {
    set_position( position );
    set_rotation( rotation );
    // Теперь корректировка положения - с тем, чтобы прижать самолет к земле.
    if ( clamp ) {
        ClampToGround();
        // Эта штука может корректировать pitch, поэтому ее в конце.
        _on_ground_correction();
    }
    // Если самолет на земле, то шасси-то у него точно выпущены же.
    v[ V_CONTROLS_GEAR_RATIO ] = 1.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Расположить самолет на стоянке.                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::place_on_ground( const startup_location_t & ramp ) {
    location_t loc;
    loc.latitude = ramp.latitude;
    loc.longitude = ramp.longitude;
    loc.altitude = 10.0; // Просто так, чтобы что-нибудь там было.
    // Переводим геолокацию в OGL игровые координаты.
    position_t position = XPlane::location_to_position( loc );
    // Угловое положение самолета.
    rotation_t rotation;
    rotation.heading = ramp.heading;
    // Первоначальная, грубая установка позиции.
    place_on_ground(position, rotation);

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
    rotation.pitch = drawInfo.pitch;
    rotation.heading = drawInfo.heading;    
    rotation.roll = drawInfo.roll;
    return rotation;    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                      Корректировка вертикального смещения, в CSL установлены не очень точно.                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::_on_ground_correction() {

    if ( acIcaoType == "B738" ) {
        drawInfo.y += 2.0; // 0.5;
        drawInfo.pitch = -1.8;
    }

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
        _params.cruise_speed = 300.0;
        _params.vertical_climb_speed = 1900.0;
        _params.vertical_descend_speed = 1400.0;
        
        __actuators[ V_CONTROLS_FLAP_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_GEAR_RATIO ].full_time = 20.0;
        __actuators[ V_CONTROLS_THRUST_RATIO ].full_time = 30.0;
        
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
    
    // В полетный план они будут вставляться в самое начало, потому что
    // полетный план может быть уже заполнен. Например, SIDом или вообще
    // полным путем. Соответственно, добавление начинается с конца полученного
    // вектора. Чтобы не извращаться - проще его реверсировать на входе.
    
    deque< waypoint_t > tw = taxi_way;
    std::reverse( tw.begin(), tw.end() );
    
    // Две начальных точки - должна быть взлетка.
    for ( int i=0; i<2; i++ ) {
        if ( tw.at( 0 ).type != WAYPOINT_RUNWAY ) {
            XPlane::log("ERROR: latest waypoints for take off is not runway!");
            return;
        }
    };
    
    // Самая первая (в инвертированном плане) точка - дальний конец ВПП.
    
    waypoint_t wp = tw.front(); tw.pop_front();
    wp.action_to_achieve = ACF_DOES_TAKE_OFF;
    wp.location.altitude = 150.0;
    _flight_plan.push_front( wp );
        
    // Вторая точка - это ближний конец ВПП. На нее выходим из состояния HP
    // выравниванием ( lining up )
    wp = tw.front(); tw.pop_front();
    wp.action_to_achieve = ACF_DOES_LINING_UP;
    _flight_plan.push_front( wp );
    
    // Дальше все точки - это рулежка.
    
    for ( int i=0; i<tw.size() - 1; i++ ) {
        wp = tw.front(); tw.pop_front();
        wp.action_to_achieve = ACF_DOES_NORMAL_TAXING;
        _flight_plan.push_front( wp );
    }
    
    // Осталась одна точка. До нее можно добраться либо выруливанием,
    // либо выталкиванием. Зависит от того, где она находится от нас,
    // спереди или сзади и можно ли до нее доехать самостоятельно.
    
    wp = tw.front(); tw.pop_front();
    location_t current_location = get_location();
    double azimuth = xenon::bearing( current_location, wp.location );
    if ( ( azimuth <= 60.0 ) || ( azimuth >= 300.0 ) )
        // Это "выруливание", потому что точка у нас перед носом.
        wp.action_to_achieve = ACF_DOES_SLOW_TAXING;
    else wp.action_to_achieve = ACF_DOES_PUSH_BACK;
    _flight_plan.push_front( wp );
    
    __taxing_prepared = true;
        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Вернуть позицию самолета в игровом пространстве                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

position_with_angles_t BimboAircraft::get_position_with_angles() {
    position_with_angles_t pos;
    pos.position = get_position();
    pos.rotation = get_rotation();
    return pos;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Изменение положения актуаторов (управляющих поверхностей)                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::__update_actuators( float elapsed_since_last_call ) { // NOLINT(bugprone-reserved-identifier)
    for ( auto i=0; i<XPMP2::V_COUNT; i++ ) {
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
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                           Переместить самолет в 3D игровых координатах согласно его курсу                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::move( float meters ) {

    auto radians = ( float ) degrees_to_radians( drawInfo.heading );
    float dx = meters * sinf( radians );
    float dz = meters * cosf( radians );
    drawInfo.x += dx;
    drawInfo.z -= dz;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                С целью тестирования, чтобы не ждать слишком долго - расположить на предварительном                *
// *                                                                                                                   *
// *********************************************************************************************************************

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

// *********************************************************************************************************************
// *                                                                                                                   *
// *        С целью тестирования - расположить самолет в конечной точке ВПП, где он должен быть уже взлетевшим         *
// *                                                                                                                   *
// *********************************************************************************************************************

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
    set_will_on_ground( false );
    place_on_ground( position, rotation, false );    
    
}


