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
        
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Внутренний старт действия                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLanding::_internal_start() {
            
    auto wp0 = _ptr_acf->flight_plan.get(0);
    if ( wp0.type != WAYPOINT_RUNWAY ) {
        _ptr_acf->flight_plan.pop_front();
        wp0 = _ptr_acf->flight_plan.get(0);
    };
        
    __phase = PHASE_DESCENDING;
    
    _ptr_acf->set_landing_lites( true );
    _ptr_acf->set_beacon_lites( true );
    _ptr_acf->set_nav_lites( true );
    _ptr_acf->set_beacon_lites( true );
                
    _ptr_acf->vcl_condition.is_clamped_to_ground = false;
        

/*    
#ifdef INSIDE_XPLANE
    
    // Если дело происходит внутри симулятора, то следующую
    // точку надо скорректировать по высоте, т.к. мы на 
    // нее собрались реально (зримо) садиться.
    
    _ptr_acf->hit_to_ground( wp0.location );
    // Плюс сколько-нибудь метров над ней, чтобы начать выравнивание и выдерживание.
    wp0.location.altitude += 8.0;
    _ptr_acf->flight_plan.set( 0, wp0 );

#endif
*/

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Один шаг в фазе снижения                                              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLanding::__step__descending( const waypoint_t & wp, const aircraft_parameters_t & acf_parameters ) {
    
    auto acf_location = _ptr_acf->get_location();
//    auto acf_rotation = _ptr_acf->get_rotation();
    
    location_t end_rwy_location = wp.location;
    
#ifdef INSIDE_XPLANE
   _ptr_acf->hit_to_ground( end_rwy_location );    
   end_rwy_location.altitude += _ptr_acf->parameters().on_ground_offset + 1.0;
#endif

   auto old_heading = _ptr_acf->get_rotation().heading;
   auto old_roll = _ptr_acf->get_rotation().roll;
   
    _head_bearing( wp );
        
    auto distance = xenon::distance2d( acf_location, wp.location );
    if ( _ptr_acf->vcl_condition.speed != 0.0 ) {
        auto time_to_achieve = distance / _ptr_acf->vcl_condition.speed;
        _altitude_adjustment( end_rwy_location.altitude, time_to_achieve );
        _speed_adjustment( xenon::knots_to_merets_per_second( wp.speed ), time_to_achieve );
    }
    
    _control_of_flaps();        
        
    // На скольки-нибудь метрах высоты относительно высоты торца ВПП - останавливаемся.
    auto da = acf_location.altitude - end_rwy_location.altitude;
    
    // Logger::log("RWY alt=" + to_string( end_rwy_location.altitude ) + ", our alt=" + to_string(acf_location.altitude) + ", da=" + to_string(da));

    if ( da <= _ptr_acf->parameters().on_ground_offset + 2.0 ) {

        // Переход в фазу выравнивания.

        Logger::log( _ptr_acf->vcl_condition.agent_name + ", jump to ALIGNMENT" );
        
        _ptr_acf->flight_plan.pop_front();
        
        __phase = PHASE_ALIGNMENT;
        
        // Убираем имеющиеся ускорения. Фаза выравнивания будет подруливать сама.
        
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
        _ptr_acf->acf_condition.target_pitch = _ptr_acf->parameters().take_off_angle;
        
        auto acf_rotation = _ptr_acf->get_rotation();        
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
    
    auto acf_position = _ptr_acf->get_position();
    
    // Где земля?
    auto ground_pos = acf_position;
    _ptr_acf->hit_to_ground( ground_pos );
    
    // Выстота (расстояние до земли)
    auto height = abs(acf_position.y - ground_pos.y);
#else
    auto acf_location = _ptr_acf->get_location();
    // Выстота (расстояние до земли)
    auto height = acf_location.altitude - wp.location.altitude;
#endif
    
    if ( height <= 0.2 ) {
        
        // Сели. Самолет прижимаем к земле.
        _ptr_acf->vcl_condition.is_clamped_to_ground = true;
        // Переходим в фазу торможения.
        __phase = PHASE_BREAKING;
        
        auto rotation = _ptr_acf->get_rotation();
        
        // При переходе в торможение сразу устанавливаем 
        // нужное нам положение тангажа и опускаем нос.
        _ptr_acf->acf_condition.target_pitch = _ptr_acf->parameters().taxing_pitch;
        if ( rotation.pitch > _ptr_acf->acf_condition.target_pitch ) _ptr_acf->acf_condition.pitch_acceleration = -0.9f;
        else _ptr_acf->acf_condition.pitch_acceleration = 0.9f;
        
        // Торможение. 
        _ptr_acf->vcl_condition.acceleration = -1.7f;
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

void AircraftDoesLanding::__step__breaking( const float & elapsed_since_last_call ) {
    
    _head_steering( elapsed_since_last_call, 10.0 );
    
    if ( _ptr_acf->vcl_condition.speed < TAXI_NORMAL_SPEED * 2.0 + 0.5 ) {
        Logger::log( _ptr_acf->vcl_condition.agent_name + ", landing done.");
        
        // Точка, на которую "целились", т.е. конечная 
        // точка ВПП в плане полета - нам больше не нужна.
        _ptr_acf->flight_plan.pop_front();
        
        // Убирание реверса.
        _ptr_acf->set_reverse_on( false );
        // Убирание воздушных тормозов.
        _ptr_acf->set_speed_brake_position( 0.0 );
        
        // Все. Действие посадки - закончено.        
        _finish();
    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Внутренний шаг дейстия                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesLanding::_internal_step( const float & elapsed_since_last_call ) {
        
    auto wp = _ptr_acf->flight_plan.get(0);
    auto acf_parameters = _ptr_acf->parameters();
    
    switch ( __phase ) {
        case PHASE_DESCENDING: __step__descending( wp, acf_parameters ); break;
        case PHASE_ALIGNMENT: __step__alignment( wp, acf_parameters, elapsed_since_last_call ); break;
        case PHASE_BREAKING: __step__breaking( elapsed_since_last_call ); break;
        default: Logger::log("UNRELEASED: does landing with phase " + to_string( __phase ) );
    };

}
