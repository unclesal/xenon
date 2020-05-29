// *********************************************************************************************************************
// *                                                    Константы.                                                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:30 *
// *********************************************************************************************************************
#pragma once
namespace xenon {

    const double PI = 3.1415926535897932384626433832795028841971693993751;
    
//     / Convert feet to meters, e.g. for altitude calculations
//     const float M_per_FT   = 0.3048;   // meter per 1 foot
//     
//     / Convert nautical miles to meters
//     const float M_per_NM      = 1852.0;
// 
    // Радиус Земли в метрах. Нужен для приблизительных вычислений внутри X-Plane.
    const double EARTH_RADIUS = 6371.0 * 1000.0;
    
    // Сильно большая константа, используется, когда ищем минимум.
    const double FAR_AWAY = (double) size_t(-1);

    // Скорость выталкивания, метров в секунду.
    // Отрицательная, потому как выталкивается-то - назад.
    const float PUSH_BACK_SPEED = -1.0;

    // Скорость при "медленной и аккуратной" рулежке, например, при заходе на стоянку.
    // const float TAXI_SLOW_SPEED = 1.9444; // 7 км/ч
    const float TAXI_SLOW_SPEED = 5.55556; // 20 км/ч

    // Скорость при "нормальной" рулежке.
    const float TAXI_NORMAL_SPEED = 11.111; // 40 км/x

    // Изменение курса самолета за секунду при рулении (скорость углового вращения).
    const float TAXI_HEADING_SHIFT_PER_SECOND = 1.8;

    // Радиус поворота при рулении. Точнее, это расстояние по прямой
    // до целевой рулежной дорожки, когда начинается поворот.
    // Конкретно "радиус" будет формироваться - скоростью движения
    // самолета + скоростью углового вращения.
    const float TAXI_TURN_RADIUS = 15.0;

    // Ускорение при старте руления или остановке.
    const float TAXI_SLOW_ACCELERATION = 0.05;
    const float TAXI_NORMAL_ACCELERATION = 1.0;
    
    // "Рывок" (производная от ускорения) при рулении или остановке.
    // const float TAXI_SLOW_TUG = 0.01;
    // const float TAXI_NORMAL_TUG = 0.1;

    // Время подъема/опускания стоек шасси.
    constexpr float TIME_FOR_GEAR_MOTION        = 10.0;

    /**
     * @short Использование ВПП.
     */
    enum runway_used_t {
        RUNWAY_USED_BOTH = 0,
        RUNWAY_USED_DEPARTURE,
        RUNWAY_USED_ARRIVAL
    };
    
    // Типы точек маршрута.
    enum waypoint_type_t {
        WAYPOINT_UNKNOWN = 0,
        WAYPOINT_PARKING,
        WAYPOINT_TAXING,
        WAYPOINT_RUNWAY,
        WAYPOINT_SID,                
        // Точка - собственно полетного плана. Вне зависимости,
        // набирает он там высоту или идет на круизе.
        WAYPOINT_FLYING,
        WAYPOINT_VECTORING,
        WAYPOINT_STAR,
        // Отдельно выделяется одна точка полета, которая является
        // точкой назначения (либо аэропорт, либо прямо торец его ВПП).
        // Для того, чтобы можно было в одно движение считать прогноз
        // по времени достижения, по началу снижения (TOD) и др.
        WAYPOINT_DESTINATION
    };
    
    // Состояния (узлы графа состояний самолета).
    enum aircraft_state_t {
        ACF_STATE_UNKNOWN = 0,
        ACF_STATE_PARKING,
        // Полная остановка (после push back, но возможно
        // и не только). И готов продолжать рулежку.
        ACF_STATE_READY_FOR_TAXING,
        // Находится на предварительном старте.
        ACF_STATE_HP,
        // Вышел на исполнительный старт, готов к взлету.
        // Между предварительным и исполнительным идет руление,
        // хотя по сути это называется line up.
        ACF_STATE_READY_FOR_TAKE_OFF,
        // Взлет произведен
        ACF_STATE_AIRBORNED,
        // Находится на глиссаде
        ACF_STATE_ON_FINAL,
        // Произвел посадку и ушел с ВПП
        ACF_STATE_RUNWAY_LEAVED
    };

    // Действия (ребра графа состояний самолета).
    enum aircraft_action_t {
        ACF_DOES_NOTHING = 0,
        // Выталкивается.
        ACF_DOES_PUSH_BACK,
        // Рулежка.
        ACF_DOES_SLOW_TAXING,
        ACF_DOES_NORMAL_TAXING,
        // Останавливается при (после) рулежке, хорошо так прямо
        // тормозит вплоть до полной его остановки.
        ACF_DOES_TAXING_STOP,

        // Скорее всего - стоит на HP. Но не факт,
        // разрешение на взлет может быть получено сразу.
        ACF_DOES_WAITING_TAKE_OFF_APPROVAL,
        // Выходит на взлетку.
        ACF_DOES_LINING_UP,
        // Выполняет взлет.
        ACF_DOES_TAKE_OFF,
        // Находится в полете. Там будут разные 
        // фазы: набор высоты, круизинг, снижение и др.
        // Но действие на все это - одно.
        ACF_DOES_FLYING,
        // Выполняет посадку (уже находится на глиссаде)
        ACF_DOES_LANDING,
        // Посадку произвел и освобождает ВПП.
        ACF_DOES_RUNWAY_LEAVING
    };


}; // namespace xenon
