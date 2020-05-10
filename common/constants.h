// *********************************************************************************************************************
// *                                                    Константы.                                                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:30 *
// *********************************************************************************************************************
#pragma once
namespace xenon {
    constexpr double PI = 3.1415926535897932384626433832795028841971693993751;

    // Скорость выталкивания, метров в секунду.
    // Отрицательная, потому как выталкивается-то - назад.
    const float PUSH_BACK_SPEED = -0.8;

    // Скорость при "медленной и аккуратной" рулежке, например, при заходе на стоянку.
    const float TAXI_SLOW_SPEED = 2.77778; // 10 км/ч

    // Скорость при "нормальной" рулежке.
    const float TAXI_NORMAL_SPEED = 5.55556; // 20 км/x

    // Изменение курса самолета за секунду при рулении (скорость углового вращения).
    const float TAXI_HEADING_SHIFT_PER_SECOND = 2.0;

    // Радиус поворота при рулении. Точнее, это расстояние по прямой
    // до целевой рулежной дорожки, когда начинается поворот.
    // Конкретно "радиус" будет формироваться - скоростью движения
    // самолета + скоростью углового вращения.
    const float TAXI_TURN_RADIUS = 22.0;

    // Ускорение при старте руления или остановке.
    const float TAXI_ACCELERATION = 0.03;

    // Время в секундах полного включения или выключения освещения.
    const float LITES_FULL_TIME = 3.0;

    /**
     * @short Использование ВПП.
     */
    enum runway_used_t {
        RUNWAY_USED_BOTH = 0,
        RUNWAY_USED_DEPARTURE,
        RUNWAY_USED_ARRIVAL
    };

//    /**
//     * @short Направление движения хвоста при выталкивании.
//     */
//
//    enum push_back_tail_direction_t {
//        PB_TAIL_STRAIGHT = 0,
//        PB_TAIL_TO_LEFT,
//        PB_TAIL_TO_RIGHT
//    };

    // Состояния "конечного автомата", в которых может находиться самолет.
    enum aircraft_does_t {
        ACF_DOES_NOTHING = 0,
        // Находится на стоянке.
        ACF_DOES_PARKING,
        // Останавливается при (после) рулежке, хорошо так прямо
        // тормозит вплоть до полной его остановки.
        ACF_DOES_TAXING_STOP,
        // Выталкивается - чисто хвостом вперед, не меняя своего курса.
        // Скорость выталкивания постоянна.
        ACF_DOES_STRAIGHT_PUSH_BACK,
        // Выталкивается - с поворотом, с изменением курса.
        // И скорость, и угловое вращение - постоянны.
        ACF_DOES_ROTATED_PUSH_BACK,
        // Рулежка.
        ACF_DOES_SLOW_TAXING,
        ACF_DOES_NORMAL_TAXING
    };

    // Битовые константы включения или выключения света.

    const unsigned short int REQUEST_LITES_NONE        = 0;
    const unsigned short int REQUEST_LITES_TAXI_ON     = 0x1;
    const unsigned short int REQUEST_LITES_TAXI_OFF    = 0x2;
    const unsigned short int REQUEST_LITES_LANDING_ON  = 0x4;
    const unsigned short int REQUEST_LITES_LANDING_OFF = 0x8;
    const unsigned short int REQUEST_LITES_BEACON_ON   = 0x10;
    const unsigned short int REQUEST_LITES_BEACON_OFF  = 0x20;
    const unsigned short int REQUEST_LITES_STROBE_ON   = 0x40;
    const unsigned short int REQUEST_LITES_STROBE_OFF  = 0x80;
    const unsigned short int REQUEST_LITES_NAV_ON      = 0x100;
    const unsigned short int REQUEST_LITES_NAV_OFF     = 0x200;


}; // namespace xenon