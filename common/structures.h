// *********************************************************************************************************************
// *                                      Структуры - общие для всего проекта.                                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:25 *
// *********************************************************************************************************************
#pragma once

#include <stdlib.h>

#include <string>
#include <functional>
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/geometries.hpp>

#include "constants.h"

using namespace std;

namespace xenon {
    // Математическое описание линии на плоскости, y = k * x + b
    struct line_descriptor_t {
        double k = 0;
        double b = 0;
    };

#ifdef INSIDE_XPLANE
    // Позиция в 3D игровом пространстве (OGL).
    struct position_t {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
    };
#endif

//    // boost-style location
//    typedef boost::geometry::model::point<
//        double, 3, boost::geometry::cs::geographic < boost::geometry::degree
//        >> boost_location_t;

//    // Стратегия для вычисления расстояния в гео-координатах.
//    typedef boost::geometry::srs::spheroid <double> spheroid_t;
//    typedef boost::geometry::strategy::distance::thomas < spheroid_t > geoid_distance_t;

//    typedef boost::geometry::model::segment<boost_location_t> boost_segment_t;

    // Локация в геосистеме
    struct location_t {
        // У этих двух double обосновано.
        double latitude = 0.0;
        double longitude = 0.0;
        // Вот ей по-идее double излишне. Но деваться некуда,
        // WorldToLocal / LocalToWorld воспринимает только double.
        double altitude = 0.0;
        // Метрика по метке L1
        double l1_flat() {
            return abs(latitude) + abs(longitude);
        };
        
        bool operator == ( const location_t & l ) {
            return (
                ( latitude == l.latitude )
                && ( longitude == l.longitude )
                && ( altitude == l.altitude )
            );
        };
    };


    // Текущие углы OGL (в 3D игровом пространстве).
    struct rotation_t {
        float pitch = 0.0;
        float heading = 0.0;
        float roll = 0.0;
    };

#ifdef INSIDE_XPLANE
    struct position_with_angles_t {
        position_t position;
        rotation_t rotation;
    };
#endif
    
    // Одна точка маршрута.
    struct waypoint_t {
        std::string name = "";
        xenon::waypoint_type_t type = WAYPOINT_UNKNOWN;
        // Положение в пространстве данной точки. Если установлена
        // высота - то пытается достигаться эта высота. Если 0, то
        // согласно фазам полетного плана.
        location_t location;
        
        // Если значение установлено - то при подходе к этой точке
        // в действии полета самолет пытается его обеспечить.
        float speed = 0.0;

        // Отдельно - курс, которым должна была быть достигнута эта точка.
        // Используется для вычисления отклонения в собственном положении.
        double incomming_heading = 0.0;
        
        // Исходящий курс, которым самолет должен выходить из этой точки.
        double outgoing_heading = 0.0;        
        
        // Дистанция (в метрах) до следующей точки.
        // У последней точки маршрута она равна нулю.
        double distance_to_next_wp = 0.0;
        // Действие для достижения этой точки плана полета.
        xenon::aircraft_action_t action_to_achieve = ACF_DOES_NOTHING;
        
        bool operator == ( const waypoint_t & w ) {
            return (                
                ( name == w.name )
                && ( type == w.type )
                && ( location == w.location )
                && ( action_to_achieve == w.action_to_achieve )
            );
        };
    };

    /**
     * @short Элемент аэропорта - стоянка.
     */
    struct startup_location_t { // code = 1300, 1301

        location_t location;

        // Latitude of location in decimal degrees Eight decimal places supported
        // double latitude = 0.0;
        // Longitude of location in decimal degrees Eight decimal places supported
        // double longitude = 0.0;

        // Heading (true) of airplane positioned at this location Decimal degrees, true heading
        float heading = 0.0;
        // Type of location “gate”, “hangar”, “misc” or “tie-down”
        string startup_type = "";
        // Airplane types that can use this location Pipe-separated list (“|”).
        // Can include “heavy”, ”jets”, “turboprops”, “props” and
        // “helos” (or just “all” for all types)
        string airplane_types = "";
        // Unique name of location Text string, must be unique within a single airport
        string name = "";
        // ICAO width code A, B, C, D, E, F
        string width_code = "";
        // Operation type none, general_aviation, airline, cargo, military
        string operation_type = "";
        // Airline permitted to use this ramp 3-letter airline codes (AAL, SWA, etc)
        string airline = "";

    };

    /**
     * @short Runway in use
     */
    struct runway_in_use_t {
        // Имя взлетки - 08R, 26L и т.д. - так, как оно есть в аэропорту.
        string name;
        // Что выполняет данная ВПП (departure, arrival)
        runway_used_t used = RUNWAY_USED_BOTH;
    };

    /**
     * @short Параметры самолета.
     * @detail По умолчанию проставлены - примерно для абстрактного небольшого винтового самолетика.
     * Для реального использования с каким-то смыслом - их сам самолет должен переустанавливать
     * сам в зависимости от конкретного типа воздушного судна.
     */
    
    struct aircraft_parameters_t {
        // Длина самолета (метров).
        float length = 6.0;
        // Размах крыла, метров.
        float wingspan = 6.0;
        // Смещение от начальной точки стоянки, метров.
        // Идет со знаком, вперед или назад смещать.
        float shift_from_ramp = -5.0;
        
        // Скорость (узлов в час), при которой происходит отрыв 
        // передней стойки шасси
        float v1 = 50.0;
        // Скорость (узлов в час), при которой начинается подъем самолета от ВПП.
        float v2 = 70.0;        
        // Скорость при наборе высоты, узлов в час.
        float climb_speed = 100.0;
        // Крейсерская скорость, узлов в час.
        float cruise_speed = 120.0;
        // Скорость при снижении.
        float descent_speed = 110.0;
        // Посадочная скорость.
        float landing_speed = 80.0;
        // Максимальная скороподьемность, футов в минуту.
        float vertical_climb_speed = 900.0;
        // Максимальная скорость снижения, футов в минуту.
        float vertical_descend_speed = 700.0;
        
        // Угол тангажа, при котором самолет стоит передней стойкой на земле.
        float taxing_pitch = 0.0;
        
        // Взлетный угол. Такой угол тангажа, при котором самолет
        // стоит максимально вверх, но хвост при этом не касается земли.
        // Используется при взлете и посадке.
        float take_off_angle = 10.0;
        
        // Смещение относительно земной поверхности в метрах,
        // чтобы шасси точно ехали по земле.
        float on_ground_offset = 2.0;
        
        // Взлетное положение закрылков, max = 1.0
        float flaps_take_off_position = 0.4;
        // Скорость, при которой нужно выпустить или убрать
        // закрылки во взлетное положение.
        float flaps_take_off_speed = 110.0;
        
        // Скорость, при которой нужно выпустить или убрать
        // закрылки в посадочное положение (1.0)
        float flaps_landing_speed = 90.0;

        // ---- Скорее, характеристики "данного полета", а не "данного самолета" -----
        
        // Высота в футах. Не является "характеристикой данного самолета", т.к.
        // устанавливается в зависимости от маршрута. Но в рамках одного полета
        // это значение можно посчитать условно-постоянным.
        float cruise_altitude = 5000.0;

        // Аэропорт отправления
        std::string departure;
        // Аэропорт назначения
        std::string destination;
        // Запасные аэропорты, заявленные в полетный план.
        std::vector<std::string> alternate;

        // Номер выполняемого рейса.
        std::string flight_number;
        
    };
    
    /**
     * @short Текущие параметры ("состояние") - любой "самоходки".
     * Структуры состояния "наземного транспортного средства" и "самолета"
     * полностью разделены (попытка поэкономить память).
     */
    struct vehicle_condition_t {
        
        agent_t agent_type = AGENT_UNKNOWN;
        
        // Кто является "автором" данного состояния.
        std::string agent_uuid;
        
        // Кому адресована информация о состоянии.
        std::string to_agent_uuid;
        
        // При передаче пакета формируется время на том агенте,
        // с которого состояние данной "самоходки" было получено.
        // Это позволяет отслеживать, насколько давно пришел пакет
        // и валидны ли данные. А так же отслеживать разницу во времени
        // на удаленном агенте и на локальном его отображении.
        long int agent_system_time_ms = 0;
        
        // name - это то, что отображается на метке объекта внутри X-Plane.
        std::string agent_name;

        // Текущее местоположение.
        location_t location;

        // Угловое положение. 
        rotation_t rotation;

        // Движение на плоскости.

        // "Рывок", производная от ускорения.
        // float tug = 0.0;
        // float target_acceleration = 0.0;

        // Ускорение, метров в секунду.
        float acceleration = 0.0;        
        // Скорость, метров в секунду.
        float speed = 0.0;
        // Скорость, узлов (морских миль) в час.
        float speed_kts = 0.0;
        // Целевая скорость, которую хотим достичь, 
        // применяя параметры "рывка" и ускорения..
        float target_speed = 0.0;
        
        // Является ли "прижатой к земле".
        bool is_clamped_to_ground = false;

        // "Курс" - это "плоскостное" понятие, т.к. 
        // он применим и к машинкам, и к самолетам.

        float heading_acceleration = 0.0;
        float target_heading = 0.0;

        // Текущее состояние графа.
        aircraft_state_t current_state = ACF_STATE_UNKNOWN;
        aircraft_action_t current_action = ACF_DOES_NOTHING;

    };
    
    /**
     * @short Текущие параметры ("состояние") - самолета.
     */
    
    struct aircraft_condition_t {
        
        std::string icao_type;
        std::string icao_airline;
        std::string livery;
        
        // Вертикальная скорость, метров в секунду.
        float vertical_speed = 0.0;
        // Целевая вертикальная скорость. При ее достижении -
        // больше не увеличивается, ускорение будет сброшено.
        float target_vertical_speed = 0.0;        
        // Вертикальное ускорение, метров в секунду.
        float vertical_acceleration = 0.0;        
        // Вертикальная скорость, футов в минуту. Пересчитывается
        // в конце "шага" в абстрактном действии, так же,
        // как и горизонтальная скорость.
        float vertical_speed_fpm = 0.0;

        // Угловые величины.        
        float pitch_acceleration = 0.0;
        float target_pitch = 0.0;
        
        float roll_acceleration = 0.0;
        float target_roll = 0.0;
        
        // Положение и состояние актуаторов.
        bool is_taxi_lites_on = false;
        bool is_landing_lites_on = false;
        bool is_beacon_lites_on = false;
        bool is_strobe_lites_on = false;
        bool is_nav_lites_on = false;
        bool is_gear_down = false;
        bool is_reverse_on = false;
        float flaps_position = 0.0;
        float speed_brake_position = 0.0;
        float thrust_position = 0.0;
        
    };

    // Описание агента с точки зрения его возможности общаться.
    struct talker_t {
        std::string uuid = "";
        agent_t type = AGENT_UNKNOWN;
    };

};  // namespace xenon
