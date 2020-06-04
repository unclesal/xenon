// *********************************************************************************************************************
// *                                              Самолет выполняет посадку                                            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 16:16 *
// *********************************************************************************************************************

#include "aircraft_does_landing.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

xenon::AircraftDoesLanding::AircraftDoesLanding(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction( ptr_acf, edge_d )
{
    __phase = PHASE_UNKNOWN;
    
    __flaps_to_take_off_position = false;
    __flaps_to_landing_position = false;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Внутренний старт действия                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLanding::_internal_start() {
    
    __phase = PHASE_UNKNOWN;
    auto wp0 = _get_front_wp();
    if ( ( wp0.type != WAYPOINT_RUNWAY ) || ( wp0.action_to_achieve != ACF_DOES_LANDING ) ) {
        Logger::log(
            "ERROR: incorrect WP for landing, type=" + to_string( wp0.type ) 
            + ", action to achieve=" + to_string( wp0.action_to_achieve ) 
        );
        return;
    };
    
    __phase = PHASE_DESCENDING;
    
    _ptr_acf->set_landing_lites( true );
    _ptr_acf->set_beacon_lites( true );
    _ptr_acf->set_nav_lites( true );
    _ptr_acf->set_beacon_lites( true );
    
    // Механизация самолета. Пока что мы еще летим.
    // _ptr_acf->v[ XPMP2::V_CONTROLS_FLAP_RATIO ] = 0.0;
    // _ptr_acf->v[ XPMP2::V_CONTROLS_SPEED_BRAKE_RATIO ] = 0.0;
    // _ptr_acf->v[ XPMP2::V_CONTROLS_GEAR_RATIO ] = 0.0;
    
    __flaps_to_take_off_position = false;
    __flaps_to_landing_position = false;
        
    _ptr_acf->vcl_condition.is_clamped_to_ground = false;
    
#ifdef INSIDE_XPLANE
    
    // Если дело происходит внутри симулятора, то следующую
    // точку надо скорректировать по высоте, т.к. мы на 
    // нее собрались реально (зримо) садиться.
    
    _ptr_acf->hit_to_ground( wp0.location );    
    // Плюс сколько-нибудь метров над ней, чтобы начать выравнивание и выдерживание.
    wp0.location.altitude += 10.0;
    _set_front_wp( wp0 );
    
#endif    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Один шаг в фазе снижения                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLanding::__step__descending( const waypoint_t & wp, const aircraft_parameters_t & acf_parameters ) {
    
    _head_bearing( wp );
    auto acf_location = _get_acf_location();
    auto acf_rotation = _get_acf_rotation();
    
    auto distance = xenon::distance2d( acf_location, wp.location );
    if ( _ptr_acf->vcl_condition.speed != 0.0 ) {
        auto time_to_achieve = distance / _ptr_acf->vcl_condition.speed;
        _altitude_adjustment( wp.location.altitude, time_to_achieve );
        _speed_adjustment( 
            xenon::knots_to_merets_per_second( acf_parameters.landing_speed ), time_to_achieve 
        );
    }
    
    // При достижении определенной скорости закрылки выпускаем во взлетное положение.
    if ( 
        ( _ptr_acf->vcl_condition.speed <= xenon::knots_to_merets_per_second( acf_parameters.flaps_take_off_speed ))
        && ( ! __flaps_to_take_off_position )
    ) {
        __flaps_to_take_off_position = true;
        _ptr_acf->set_flaps_position( acf_parameters.flaps_take_off_position );
        Logger::log("Flaps to TO position");
    }
    
    // Если скорость еще снизилась, то закрылки выпускаем в посадочное положение 
    
    if (
        ( _ptr_acf->vcl_condition.speed <= xenon::knots_to_merets_per_second( acf_parameters.flaps_landing_speed ))
        && ( ! __flaps_to_landing_position )
    ) {
        Logger::log("Flaps to LAND position");
        __flaps_to_landing_position = true;
        _ptr_acf->set_flaps_position( 1.0 );
        _ptr_acf->set_gear_down( true );                        
    }     
    
    
    if ( __flaps_to_landing_position ) {
        
        // Если закрылки выпущены в посадочное положение, то считаем, что самолет летит 
        // уже достаточно медленно. Поэтому выравнивается на положительный тангаж.
        // При этом будут перекрываться установки по тангажу _altitude_adjustment.
        
        _ptr_acf->acf_condition.target_pitch = 5.0;
        
        // Нос пошел вверх не спеша. Торопиться некуда: это все происходит довольно далеко от ВПП.
        
        if ( acf_rotation.pitch > _ptr_acf->acf_condition.target_pitch ) 
            _ptr_acf->acf_condition.pitch_acceleration = -0.2f;
        else
            _ptr_acf->acf_condition.pitch_acceleration = 0.2f;
    }
    
    location_t end_rwy_location = wp.location;

#ifdef INSIDE_XPLANE    
    _ptr_acf->hit_to_ground( end_rwy_location );
#endif

    // На скольки-нибудь метрах высоты относительно высоты торца ВПП - останавливаемся.
    auto da = acf_location.altitude - end_rwy_location.altitude;

    if ( da <= 13.0 ) {

        // Переход в фазу выравнивания.

        Logger::log( "jump to ALIGNMENT" );
        
        _front_wp_reached();
        __phase = PHASE_ALIGNMENT;
        
        // Убираем имеющиеся ускорения. Фаза выравнивания будет подруливать сама.
        
        // _params.tug = 0.0;
        _ptr_acf->vcl_condition.acceleration = 0.0;        
        
        _ptr_acf->acf_condition.target_vertical_speed = -1.0;
        if ( _ptr_acf->acf_condition.vertical_speed > _ptr_acf->acf_condition.target_vertical_speed ) _ptr_acf->acf_condition.vertical_acceleration = -2.0f;
        else _ptr_acf->acf_condition.vertical_acceleration = 2.0f;
                
        // По крену - насмерть. Как есть - так и есть.
        _ptr_acf->acf_condition.roll_acceleration = 0.0;

        // Курс - оставляем как есть, не меняем. Потому что в 
        // фазе выравнивания тоже будет подруливание.
                
        // Целевой угол, на который мы должны выйти - это достаточно большой 
        // угол, но такой, при котором хвост еще не касается земли.        
        _ptr_acf->acf_condition.target_pitch = _get_acf_parameters().take_off_angle;
        
        auto acf_rotation = _get_acf_rotation();        
        if ( acf_rotation.pitch < _ptr_acf->acf_condition.target_pitch ) _ptr_acf->acf_condition.pitch_acceleration = 2.0f;
         else _ptr_acf->acf_condition.pitch_acceleration = 2.0f;
        
        return;
    };    

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Один шаг в фазе выравнивания                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLanding::__step__alignment( 
    const waypoint_t & wp, const aircraft_parameters_t & acf_parameters, const float & elapsed_since_last_call 
) {
    
    // Осуществляем подруливание по курсу, но уже в "автомобильном" 
    // стиле, оставляя при этом положение самолета неподвижным.
    _head_steering(elapsed_since_last_call, 15.0 );

#ifdef INSIDE_XPLANE
    
    auto acf_position = _get_acf_position();
    
    // Где земля?
    auto ground_pos = acf_position;
    _ptr_acf->hit_to_ground( ground_pos );
    

    // auto target_pos = XPlane::location_to_position( wp.location );
    // auto distance = XPlane::distance_2d( acf_position, target_pos );
    
    // Выстота (расстояние до земли)
    auto height = acf_position.y - ground_pos.y;
#else
    auto acf_location = _get_acf_location();
    // Выстота (расстояние до земли)
    auto height = acf_location.altitude - wp.location.altitude;
#endif
    
    if ( height <= 0.3 ) {
        
        Logger::log("Touch down!!!");
        // Сели. Самолет прижимаем к земле.
        _ptr_acf->vcl_condition.is_clamped_to_ground = true;
        // Переходим в фазу торможения.
        __phase = PHASE_BREAKING;
        
        auto rotation = _get_acf_rotation();
        
        // При переходе в торможение сразу устанавливаем 
        // нужное нам положение тангажа и опускаем нос.
        _ptr_acf->acf_condition.target_pitch = _get_acf_parameters().taxing_pitch;
        if ( rotation.pitch > _ptr_acf->acf_condition.target_pitch ) _ptr_acf->acf_condition.pitch_acceleration = -0.9f;
        else _ptr_acf->acf_condition.pitch_acceleration = 0.9f;
        
        // Торможение.
        _ptr_acf->vcl_condition.acceleration = -3.0f;
        // До нуля-то не надо, быстрее освободим ВПП.
        _ptr_acf->vcl_condition.target_speed = TAXI_NORMAL_SPEED;
        
        // Включение реверса.
        _ptr_acf->set_reverse_on( true );
        // Выпуск воздушных тормозов.
        _ptr_acf->set_speed_brake_position( 1.0 );
            
    } else if ( height <= 2.5 ) {
        
        // Достаточно высоко - снижаемся поинтенсивнее.
        _ptr_acf->acf_condition.target_vertical_speed = -0.6f;

        if ( _ptr_acf->acf_condition.vertical_speed < _ptr_acf->acf_condition.target_vertical_speed )
            // Цифра глаза режет с учетом вышестоящей целевой
            // вертикальной скорости, но проблема в том, что самолет
            // может иметь - не известную нам вертикальную скорость.
            _ptr_acf->acf_condition.vertical_acceleration = 2.0f;
        else
            _ptr_acf->acf_condition.vertical_acceleration = -2.0f;
        
    }
        
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Шаг в фазе торможения                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLanding::__step__breaking() {
    
    if ( _ptr_acf->vcl_condition.speed < TAXI_NORMAL_SPEED + 0.5 ) {
        Logger::log("DONE");
        // Точка, на которую "целились", т.е. конечная 
        // точка ВПП в плане полета - нам больше не нужна.
        _front_wp_reached();
        
        // Убирание реверса.
        _ptr_acf->set_reverse_on( false );
        // Убирание воздушных тормозов.
        _ptr_acf->set_speed_brake_position( 0.0 );
        
        // Все. Действие посадки - закончено.
        auto location = _ptr_acf->get_location();
        Logger::log("Stopped lat=" + to_string( location.latitude ) + ", lon=" + to_string(location.longitude));
        _finish();
    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Внутренний шаг дейстия                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLanding::_internal_step( const float & elapsed_since_last_call ) {
    
    auto wp = _get_front_wp();
    auto acf_parameters = _get_acf_parameters();    
    switch ( __phase ) {
        case PHASE_DESCENDING: __step__descending( wp, acf_parameters ); break;
        case PHASE_ALIGNMENT: __step__alignment( wp, acf_parameters, elapsed_since_last_call ); break;
        case PHASE_BREAKING: __step__breaking(); break;
        default: Logger::log("UNRELEASED: does landing with phase " + to_string( __phase ) );
    };

}
