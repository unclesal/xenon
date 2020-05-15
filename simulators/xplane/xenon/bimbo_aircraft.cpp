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

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                            Действие было завершено, переход в следующее состояние                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::does_finished( void * action ) {
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
    waypoint_t wp;    
    wp.location = get_location();
    wp.rotation = get_rotation();
    wp.type = WAYPOINT_PARKING;
    __graph->place_on_parking( wp );

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
        drawInfo.y += 0.3;
        drawInfo.pitch = -1.8;
    }
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
// *            Подготовить маршрут для выталкивания или руления на начальную точку рулежки к ВПП вылета               *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void BimboAircraft::prepare_for_push_back_or_taxing( const location_with_angles_t & target ) {
    location_t current_location = get_location();
    double bearing = XPlane::bearing(current_location, target.location);
    // Если в обзоре +- 45 градусов - то поедем. Если нет - то будем выталкиваться.
    if (( bearing <= 45.0 ) || ( bearing >= (360 - 45.0))) _prepare_for_taxing( target );
    else _prepare_for_push_back( target );
    // В первой фазе, вне зависимости от того, поедем мы вперед
    // или назад, нужно предусмотреть включение огней.
//    if ( ! _conditions.empty() ) {
//        _conditions.at(0).request_lites = REQUEST_LITES_BEACON_ON | REQUEST_LITES_NAV_ON;
//    }
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                        Подготовить маршрут для "штатной" рулежки с включенным двигателем.                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::prepare_for_taxing( const vector<location_t> & taxi_way ) {

    /*
    for ( const auto & point : taxi_way ) {
        aircraft_condition_t c;
        c.does = ACF_DOES_NORMAL_TAXING;
        c.tug = 0.2;
        c.target_acceleration = (float) 1.0;
        c.target_speed = TAXI_NORMAL_SPEED;
        // c.heading = point.rotation.heading;
        position_with_angles_t pwa;
        pwa.position = XPlane::location_to_position( point.location );
        pwa.rotation = point.rotation;
        c.target_pwa = pwa;
        c.exit = [](
            aircraft_condition_t & current_condition
        ) {
            auto distance = XPlane::distance_2d(
                current_condition.current_pwa.position,
                current_condition.target_pwa.position
            );

            // Курс от текущего положения на целевую точку.
            auto current_bearing = XPlane::bearing(
                current_condition.current_pwa.position,
                current_condition.target_pwa.position
            );

            // Текущий курс.
            auto current_heading = current_condition.current_pwa.rotation.heading;

            // Разность между текущим и желаемым курсом
            auto delta_bearing = current_bearing - current_heading;

            current_condition.heading_shift = 0.0;

            if ( abs(delta_bearing) >= 1.0 ) {
                if ( delta_bearing < 0 ) current_condition.heading_shift = -TAXI_HEADING_SHIFT_PER_SECOND;
                else current_condition.heading_shift = TAXI_HEADING_SHIFT_PER_SECOND;
            };

            XPlane::log(
                "current course=" + to_string( current_heading )
                + ", azimuth=" + to_string( current_bearing )
                // + ", target_bearing=" + to_string( current_condition.target_pwa.rotation.heading )
                + ", delta=" + to_string(delta_bearing)
                + ", heading shift=" + to_string(current_condition.heading_shift)
            );


            if (
                ( current_condition.previous_position_distance_to_target != 0.0 )
                && ( current_condition.previous_position_distance_to_target < distance )
            ) {
                // Мы начали удаляться от планируемой точки. Дистанция сейчас больше,
                // чем она была в прошлый раз. Выходим.
                return true;
            }

            if ( distance <= 5.0 ) return true;
            current_condition.previous_position_distance_to_target = distance;
            return false;

        };
        _conditions.push_back(c);
    }
    */
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                  Реализация подготовки к самостоятельному рулению на стартовую точку для вылета                   *
// *                                                                                                                   *
// *********************************************************************************************************************

//void BimboAircraft::_prepare_for_taxing( const location_with_angles_t & target ) {
//    XPlane::log("BimboAircraft::_prepare_for_taxing: not realized.");
//}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                       Реализация подготовки к выталкиванию на стартовую точку для вылета                          *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void BimboAircraft::_prepare_for_push_back( const location_with_angles_t & target ) {

    // Первая фаза. Прямолинейное движение тем же самым курсом до тех пор, пока
    // расстояние от места положения самолета до прямой, образованной
    // целевой точкой и целевым курсом, не станет меньше TAXI_TURN_RADIUS.
    aircraft_condition_t phase1 = _make_condition_straight_push_back( target );
    _conditions.push_back( phase1 );

    // Фаза 2. Сохраняя ту же самую скорость - меняем курс до тех пор, пока он не станет "близко" к нужному.
    aircraft_condition_t phase2 = _make_condition_rotated_push_back( target );
    _conditions.push_back( phase2 );

    // Фаза 3 - плавная остановка с сохранением курса.
    aircraft_condition_t phase3 = _make_condition_full_taxing_stop(PUSH_BACK_SPEED);
    _conditions.push_back(phase3);

}
*/

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Реализация прямго выталкивания                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

/*
aircraft_condition_t BimboAircraft::_make_condition_straight_push_back( const location_with_angles_t & target ) {

    aircraft_condition_t cond;
    cond.does = ACF_DOES_STRAIGHT_PUSH_BACK;
    cond.tug = - 0.005;
    cond.target_acceleration = -TAXI_ACCELERATION;
    cond.target_speed = PUSH_BACK_SPEED;
    cond.target_pwa.position = XPlane::location_to_position(target.location);
    cond.target_pwa.rotation.heading = target.rotation.heading;

    cond.exit = []( aircraft_condition_t & current_condition ) {
        // Описатель "целевой рулежной дорожки", куда мы собрались приехать.
        line_descriptor_t final_runway = XPlane::line(
            current_condition.target_pwa.position,
            current_condition.target_pwa.rotation.heading
        );

        // "Плоское" расстояние от места положения самолета до этой самой прямой линии.
        double distance = XPlane::distance_2d(
            current_condition.current_pwa.position, final_runway
        );

        return ( distance <= TAXI_TURN_RADIUS );

    };
    return cond;

}
*/

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Реализация выталкивания с поворотом                                          *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
aircraft_condition_t BimboAircraft::_make_condition_rotated_push_back( const location_with_angles_t & target ) {
    aircraft_condition_t phase;
    phase.does = ACF_DOES_ROTATED_PUSH_BACK;
    phase.speed = PUSH_BACK_SPEED;
    phase.target_pwa.rotation.heading = target.rotation.heading;
    // На вопрос, в какую сторону будем поворачивать, вправо или влево - сейчас
    // ответить нельзя, потому что мы не знаем текущий курс самолета. Поэтому
    // сам поворот будет формироваться - внутри фазы.

    phase.exit = [](
        aircraft_condition_t & current_condition
    ) {
        // Текущий курс самолета.
        double heading = current_condition.current_pwa.rotation.heading;
        normalize_degrees( heading );
        // Целевой курс, как нам надо встать.
        double target_heading = current_condition.target_pwa.rotation.heading;
        normalize_degrees( target_heading );

        double delta_heading = target_heading - heading;
        // До нормализации - если стало "достаточно близко" к целевому курсу.
        if ( abs(delta_heading) <= TAXI_HEADING_SHIFT_PER_SECOND ) return true;

        // Нормализация, потому что сейчас будем определять сторону поворота.
        normalize_degrees( delta_heading );

        // В какую сторону поворачиваем? Оно как бы несколько излишне, потому что
        // будет устанавливаться каждый раз в функции определения своего положения.
        // Но раньше его было сделать - нельзя, т.к. не был известен текущий курс.
        delta_heading < 180.0 ?
            current_condition.heading_shift = TAXI_HEADING_SHIFT_PER_SECOND
                              : current_condition.heading_shift = - TAXI_HEADING_SHIFT_PER_SECOND;

//        XPlaneUtilities::log(
//            "PH2: current heading=" + to_string( heading )
//            + ", target heading=" + to_string( target_heading )
//            + ", delta heading=" + to_string( delta_heading )
//        );

        return false;

    };
    return phase;

}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Полная остановка при рулежке                                               *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
aircraft_condition_t BimboAircraft::_make_condition_full_taxing_stop(const float &from_speed) {

    aircraft_condition_t ph;
    ph.speed = from_speed;
    ph.does = ACF_DOES_TAXING_STOP;
    ph.exit = [](
        aircraft_condition_t & current_condition
    ) {

        if ( abs(current_condition.speed) <= 2 * TAXI_ACCELERATION ) {
            current_condition.speed = 0.0;
            return true;
        }

        if (current_condition.speed > 0 ) {
            // current_condition.tug = -0.005;
            // current_condition.target_acceleration = - TAXI_ACCELERATION;
            current_condition.speed -= TAXI_ACCELERATION;
        } else {
            // current_condition.tug = 0.005;
            // current_condition.target_acceleration = TAXI_ACCELERATION;
            current_condition.speed += TAXI_ACCELERATION;
        }

        return false;

    };

    return ph;
}
*/

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
// *                  Управление (приращение или уменьшение значения) для одного источника света                       *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void BimboAircraft::_control_one_light( float & value, const float & dv ) {
    if ( dv > 0 ) {
        // Идем в увеличение.
        if ( value < 1.0 ) value += dv;
        if ( value > 1.0 ) value = 1.0;
    } else {
        // Идем в уменьшение.
        if ( value > 0.0 ) value += dv;
        if ( value < 0.0 ) value = 0.0;
    }
}
*/
// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Управление освещением самолета                                              *
// *                                                                                                                   *
// *********************************************************************************************************************
/*
void BimboAircraft::_control_request_lites( const unsigned int & request, const float & elapsed_since_last_call ) {
    if ( request == REQUEST_LITES_NONE ) return;
    float dl_plus = elapsed_since_last_call / LITES_FULL_TIME;
    float dl_minus = - dl_plus;
    if ( request & REQUEST_LITES_TAXI_ON ) _control_one_light( v[ V_CONTROLS_TAXI_LITES_ON ], dl_plus );
    if ( request & REQUEST_LITES_TAXI_OFF ) _control_one_light( v[ V_CONTROLS_TAXI_LITES_ON ], dl_minus );
    if ( request & REQUEST_LITES_LANDING_ON ) _control_one_light( v[ V_CONTROLS_LANDING_LITES_ON ], dl_plus );
    if ( request & REQUEST_LITES_LANDING_OFF ) _control_one_light( v[ V_CONTROLS_LANDING_LITES_ON ], dl_minus );
    if ( request & REQUEST_LITES_BEACON_ON ) _control_one_light( v[ V_CONTROLS_BEACON_LITES_ON ], dl_plus );
    if ( request & REQUEST_LITES_BEACON_OFF ) _control_one_light( v[ V_CONTROLS_BEACON_LITES_ON ], dl_minus );
    if ( request & REQUEST_LITES_STROBE_ON ) _control_one_light( v[ V_CONTROLS_STROBE_LITES_ON ], dl_plus );
    if ( request & REQUEST_LITES_STROBE_OFF ) _control_one_light( v[ V_CONTROLS_STROBE_LITES_ON ], dl_minus );
    if ( request & REQUEST_LITES_NAV_ON ) _control_one_light( v[ V_CONTROLS_NAV_LITES_ON ], dl_plus );
    if ( request & REQUEST_LITES_NAV_OFF ) _control_one_light( v[ V_CONTROLS_NAV_LITES_ON ], dl_minus );
}
*/
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
                if ( abs(v[i]) >= abs(__actuators[i].endpoint) ) {
                    // Достигли конечной точки.
                    v[i] = __actuators[i].endpoint;
                    __actuators[i].requested = false;
                }
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

    /*
    // Подсчет полного времени выполнения данной фазы.
    _current_condition.duration += elapsed_since_last_call;

    // Рывок. Если он есть и ускорение еще не достигло целевого значения - то наращиваем ускорение.
    if (
            ( _current_condition.tug != 0.0 ) && (
                ( abs(_current_condition.acceleration) < abs(_current_condition.target_acceleration) )
            )
    ) _current_condition.acceleration += _current_condition.tug * elapsed_since_last_call;

    // Ускорение. Если оно есть скорость еще не достигла целевого значения - то наращиваем скорость.
    if (
            ( _current_condition.acceleration != 0.0 ) && (
                ( abs(_current_condition.speed) < abs(_current_condition.target_speed) )
            )
    ) _current_condition.speed += _current_condition.acceleration * elapsed_since_last_call;

    // Дистанция, на которую надо подвинуть модельку.
    float distance_in_meters = _current_condition.speed * elapsed_since_last_call;
    move( distance_in_meters );
    _current_condition.distance += abs(distance_in_meters);

    // Курс (угловое положение модели)
    if ( _current_condition.heading_shift != 0.0 ) {
        double next_heading = drawInfo.heading + _current_condition.heading_shift * elapsed_since_last_call;
        normalize_degrees( next_heading );
        drawInfo.heading = ( float ) next_heading;
    }

    // Обработка функции перехода на следующую фазу.
    _current_condition.current_pwa = get_position_with_angles();
    if ( _current_condition.exit(_current_condition)) apply_next_condition();
    */

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
// *                                    Включение / выключение рулежных огней                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::set_taxi_lites(bool on) {
    on ? v[ V_CONTROLS_TAXI_LITES_ON ] = 1.0 : v[ V_CONTROLS_TAXI_LITES_ON ] = 0.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Включение / выключение посадочных огней                                         *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::set_landing_lites(bool on) {
    on ? v[ V_CONTROLS_LANDING_LITES_ON ] = 1.0 : v[ V_CONTROLS_LANDING_LITES_ON ] = 0.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                         Включение / выключение проблескового маячка (anti-collision)                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::set_beacon_lites(bool on) {    
    on ? v[ V_CONTROLS_BEACON_LITES_ON ] = 1.0 : v[ V_CONTROLS_BEACON_LITES_ON ] = 0.0;    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                              Включение / выключение навигационного стробоскопа                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::set_strobe_lites(bool on) {    
    on ? v[ V_CONTROLS_STROBE_LITES_ON ] = 1.0 : v[ V_CONTROLS_STROBE_LITES_ON ] = 0.0;    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                    Включение / выключение навигационных огней                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void BimboAircraft::set_nav_lites(bool on) {
    on ? v[ V_CONTROLS_NAV_LITES_ON ] = 1.0 : v[ V_CONTROLS_NAV_LITES_ON ] = 0.0;
}
