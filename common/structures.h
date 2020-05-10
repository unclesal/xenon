// *********************************************************************************************************************
// *                                      Структуры - общие для всего проекта.                                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:25 *
// *********************************************************************************************************************
#pragma once
#include <string>
#include <functional>
#include "constants.h"

using namespace std;

namespace xenon {
    // Математическое описание линии на плоскости, y = k * x + b
    struct line_descriptor_t {
        double k = 0;
        double b = 0;
    };

    // Позиция в 3D игровом пространстве (OGL).
    struct position_t {
        double x = 0.0;
        double y = 0.0;
        double z = 0.0;
    };

    // Локация в геосистеме
    struct location_t {
        double latitude = 0.0;
        double longitude = 0.0;
        double altitude = 0.0;
    };

    // Текущие углы OGL (в 3D игровом пространстве).
    struct rotation_t {
        float pitch = 0.0;
        float heading = 0.0;
        float roll = 0.0;
    };

    struct position_with_angles_t {
        position_t position;
        rotation_t rotation;
    };

    struct location_with_angles_t {
        location_t location;
        rotation_t rotation;
    };

    /**
     * @short Элемент аэропорта - стоянка.
     */
    struct startup_location_t { // code = 1300, 1301
        // Latitude of location in decimal degrees Eight decimal places supported
        double latitude = 0.0;
        // Longitude of location in decimal degrees Eight decimal places supported
        double longitude = 0.0;
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

    // Характеристики самолета.
    struct aircraft_condition_t {
        // Фаза, в которой находится самолет.
        aircraft_does_t does = ACF_DOES_NOTHING;
        // По какой рулежке или воздушной трассе двигаемся.
        string way = "";
        // Скорость, метров в секунду.
        float speed = 0.0;
        // Целевая скорость. Если положительная, то дальше
        // изменяться на основе ускорения - не будет.
        float target_speed = 0.0;
        // Ускорение, метров в секунду за секунду.
        float acceleration = 0.0;
        // Целевое ускорение. Если не равно нулю, то дальше
        // изменяться на основе "рывка" оно не будет.
        float target_acceleration = 0.0;
        // "рывок", третья производная от скорости
        float tug = 0.0;
        // Скорее всего, потом надо убрать.
        // float heading = -1;

        // Угловое смещение по курсу в градусах за секунду.
        float heading_shift = 0.0;
        // Сколько уже пройдено в метрах в данном сегменте.
        double distance = 0.0;
        // Время в секундах продолжительности работы данной фазы.
        double duration = 0.0;
        // Текущее положение самолета в 3D игровом пространстве.
        position_with_angles_t current_pwa;
        // Положение самолета или некой конечной точки,
        // заданное здесь как параметр, обрабатываемый в exit_function.
        position_with_angles_t target_pwa;
        // Для внутреннего подсчета оставшейся дистанции до конечной точки.
        double previous_position_distance_to_target = 0.0;

        // Запрос на включение или выключение света. Происходит -
        // разово, без всякой задержки, в момент переключения фазы.
        unsigned short int request_lites = REQUEST_LITES_NONE;

        // Функция перехода на следующее состояние.
        std::function<bool(
            aircraft_condition_t & condition
        )>
            exit = [](
                aircraft_condition_t & condition
            ) { return false; };

    };

};  // namespace xenon
