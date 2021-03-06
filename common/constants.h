// *********************************************************************************************************************
// *                                                    Константы.                                                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:30 *
// *********************************************************************************************************************
#pragma once
namespace xenon {

    const double PI = 3.1415926535897932384626433832795028841971693993751;
    
    // Радиус Земли в метрах. Нужен для приблизительных вычислений внутри X-Plane.
    const double EARTH_RADIUS = 6371.0 * 1000.0;
    
    // Сильно большая константа, используется, когда ищем минимум.
    const double FAR_AWAY = (double) size_t(-1);

    // Скорость выталкивания, метров в секунду.
    // Отрицательная, потому как выталкивается-то - назад.
    const float PUSH_BACK_SPEED = -0.8;
    
    const float PARKING_SPEED = 1.5;

    // Скорость при "медленной и аккуратной" рулежке, например, при заходе на стоянку.
    const float TAXI_SLOW_SPEED = 4.16667;  // 15 км/ч

    // Скорость при "нормальной" рулежке.
    const float TAXI_NORMAL_SPEED = 11.111; // 40 км/x

    // Изменение курса самолета за секунду при рулении (скорость углового вращения).
    const float TAXI_HEADING_SHIFT_PER_SECOND = 1.8;

    // Радиус поворота при рулении. 
    const float TAXI_TURN_RADIUS = 13.0;

    // Ускорение при старте руления или остановке.
    const float TAXI_SLOW_ACCELERATION = 0.4;
    const float TAXI_NORMAL_ACCELERATION = 0.8;
    
    // Радиус круга (в метрах), в котором два агента могут еще "слышать" друг друга.
    // Если расстояние больше, то агент уже не может "слышать" и пакет ему переправлен не будет.
    constexpr double DISTANCE_STILL_HEARD           = 90000.0;
    
    // Минимально допустимая дистанция между самолетами в метрах. 
    // Если меньше или равно, то задний должен остановиться.
    constexpr float MIN_ALLOWABLE_TAXING_DISTANCE = 120.0f;
    
    // Минимально допустимая дистанция между самолетами для выталкивания.
    // Сделана довольно большой, чтобы выталкивались - по одному, а не кучкой.
    // На самом деле это "костыль", т.к. выталкивать должен - tug, а он сам разберется.
    constexpr float MIN_ALLOWABLE_PUSH_BACK_DISTANCE = 400.0f;
    
    // Задний самолет прекратит руление, если на этой дистанции и ближе выполняется push back.
    // Чтобы дать возможность корректно завершится выталкиванию.
    constexpr float MIN_PUSH_BACK_AHEAD_ME = 250.0f;
    
    // Дистанция от самолета на HP до самолета, находящегося на ВПП, при которой мы
    // считаем, что исполнительный старт - занят.
    constexpr float MIN_HP_LU_OCCUPATED = 250.0f;
    
    // Минимально допустимая дистанция до ранее взлетевшего самолета.
    constexpr float MIN_ALLOWABLE_TAKE_OFF_DISTANCE = 6000.0f;
    
    // Минимально допустимая дистанция между самолетами в метрах - в воздухе.
    constexpr float MIN_ALLOWABLE_FLYING_DISTANCE = 10000.0f;
    
    // Расстояние в метрах, при котором в полете мы считаем, что данная точка полетного плана достигнута.
    constexpr double FLY_WAYPOINT_REACHED_DISTANCE = 1000.0;
    
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
        WAYPOINT_HP,
        WAYPOINT_RUNWAY,
        WAYPOINT_RUNWAY_LEAVED,
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
        // Началось движение (ушел с парковки)
        ACF_STATE_MOTION_STARTED,
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
        // Находится в состоянии полета.
        ACF_STATE_ON_THE_FLY,
        // Зашел в растр ILS, снижение и посадка, отсюда идет gliding.
        ACF_STATE_APPROACH,
        // Находится на последнем этапе посадочной прямой, отсюда идет landing.
        ACF_STATE_ON_FINAL,
        // Произвел посадку, закончил торможение.
        ACF_STATE_LANDED,
        // Освободил ВПП
        ACF_STATE_RUNWAY_LEAVED,
        // Непосредственно перед тем, как запарковаться
        // (в плане полета осталась одна точка, это сама парковка)
        ACF_STATE_BEFORE_PARKING
    };

    // Действия (ребра графа состояний самолета).
    enum aircraft_action_t {
        ACF_DOES_NOTHING = 0,
        // Ожидает выталкивания.
        ACF_DOES_WAITING_PUSH_BACK,
        // Начал движение
        ACF_DOES_START_MOTION,
        // Выталкивается.
        ACF_DOES_PUSH_BACK,
        // Рулежка.
        ACF_DOES_SLOW_TAXING,
        ACF_DOES_NORMAL_TAXING,
        // Останавливается при (после) рулежке, хорошо так прямо
        // тормозит вплоть до полной его остановки.
        ACF_DOES_TAXING_STOP,

        // Скорее всего - стоит на HP или LU. Но не факт,
        // разрешение на взлет может быть получено сразу.
        ACF_DOES_WAITING_TAKE_OFF_APPROVAL,
        // Выходит на взлетку.
        ACF_DOES_LINING_UP,
        // Выполняет взлет.
        ACF_DOES_TAKE_OFF,
        // После взлета еще какое-то время самолет
        // выполняет "становление", после чего переходит
        // в состояние полета.
        ACF_DOES_BECOMING,
        // Находится в полете. Там будут разные 
        // фазы: набор высоты, круизинг, снижение и др.
        // Но действие на все это - одно.
        ACF_DOES_FLYING,
        // Заход в растр ILS и снижение (на глиссаде)
        ACF_DOES_GLIDING,
        // Выполняет посадку (10 км и ближе от ВПП)
        ACF_DOES_LANDING,
        // Посадку произвел и освобождает ВПП.
        ACF_DOES_RUNWAY_LEAVING,
        ACF_DOES_PARKING
    };
    
    // Типы возможных агентов
    enum agent_t {
        AGENT_UNKNOWN = 0,
        // X-Plane - это тоже агент.
        AGENT_XPLANE,
        AGENT_AIRCRAFT,
        AGENT_AIRPORT,
        // Тягач, он же "толкач".
        AGENT_PUSHER
    };
    
    // Кому адресован пакет.
    enum say_to_t {
        SAY_TO_UNKNOWN = 0,
        SAY_TO_NOBODY,
        // Отправить всем в "окружающей действительности", т.е. 
        // по принципу слышит или нет. Если слышит - то получит,
        // независимо от команды, группы и др.
        SAY_TO_ENVIRONMENT,
        SAY_TO_THIS_TYPE,
        SAY_TO_THIS_ONE        
    };




}; // namespace xenon
