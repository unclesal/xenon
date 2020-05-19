// *********************************************************************************************************************
// *                          Интерфейс некоего абстрактного действия (ребро графа состояний самолета).                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:04 *
// *********************************************************************************************************************

#include "aircraft_abstract_action.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор.                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftAbstractAction::AircraftAbstractAction (
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) {
    _edge_d = edge_d;
    _ptr_acf = ptr_acf;
        
    __total_duration = 0.0;
    __started = false;
    __finished = false;
    
    __previous_distance_to_front_wp = 0;

}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                              Старт данного действия                                              *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__start() {
    
    if ( ! __started ) {
        
        // Установка тех переменных, которые суммируются в ходе выполнения данного действия. Одно
        // и то же действие может быть вызвано - неоднократно. Поэтому при старте их надо обнулять.
        
        __total_duration = 0.0;
        __total_distance = 0.0;
        __started = true;                
        
        auto wp = _get_front_wp();
        __previous_distance_to_front_wp = (int) _calculate_distance_to_wp( wp );
        
        _internal_start();
        
    };
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                               Управление скоростью                                               *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__control_of_speed( const float & elapsed_since_last_call ) {
    
    if ( _params.tug != 0.0 ) {
        if ( abs( _params.acceleration ) != abs( _params.target_acceleration )) {
            _params.acceleration += _params.tug * elapsed_since_last_call;
            if ( _params.tug < 0.0 ) {
                // "Рывок" - ниже нуля. Ускорение может стать - меньше заданного.
                if ( _params.acceleration < _params.target_acceleration ) 
                    _params.acceleration = _params.target_acceleration;
            } else {
                // "Рывок" - больше нуля. Ускорение может стать - больше заданного.
                if ( _params.acceleration > _params.target_acceleration ) 
                    _params.acceleration = _params.target_acceleration;
            };
        }
    }
    
    if ( abs( _params.speed ) != abs( _params.target_speed ) ) {
        _params.speed += _params.acceleration * elapsed_since_last_call;
        
        if ( _params.acceleration < 0.0 ) {
            // Ускорение - меньше нуля. Скорость может стать - ниже заданной.
            if ( _params.speed < _params.target_speed ) _params.speed = _params.target_speed;
        } else {
            // Ускорение - больше нуля. Скорость может стать - выше заданной.
            if ( _params.speed > _params.target_speed ) _params.speed = _params.target_speed;
        }        
    }
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                         Прямолинейное перемещение самолета                                       *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__move_straight( const float & elapsed_since_last_call ) {
    
    double distance = _params.speed * elapsed_since_last_call;
    _ptr_acf->move( distance );
    
    // Пройденная дистанция в любом случае увеличивается, даже если
    // самолет при этом двигается назад (при его выталкивании).
    __total_distance += abs(distance);
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                       Управление угловым положением самолета                                     *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__control_of_angles( const float & elapsed_since_last_call ) {
    
    auto rotation = _ptr_acf->get_rotation();
    
    if ( ( _params.heading_acceleration != 0.0 ) && ( abs( _params.speed ) >= 0.8 ) ) {    
        
        double heading = rotation.heading;

// Не надо его выключать, пусть подруливает вообще постоянно.        
//         double dh = heading - _params.target_heading;
//         normalize_degrees( dh );
//         if ( abs(dh) < 0.25 ) {
//             _params.heading_acceleration = 0.0;
//             return;
//         };
        
        heading += _params.heading_acceleration * elapsed_since_last_call;
        normalize_degrees( heading );
        rotation.heading = heading;
        
//         Конечное положение курса.
//         
//         if ( _params.heading_acceleration < 0.0 ) {
//             Отрицательное приращение, курс может уйти - ниже, чем целевой.
//             if ( rotation.heading < _params.target_heading ) {
//                 rotation.heading = _params.target_heading;
//                 _params.heading_acceleration = 0.0;
//             }
//         } else {
//             Положительное приращение. Курс если уйдет, то будет больше, чем целевой.
//             if ( rotation.heading > _params.target_heading ) {
//                 rotation.heading = _params.target_heading;
//                 _params.heading_acceleration = 0.0;
//             }
//         }

        _ptr_acf->set_rotation( rotation );
    }
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                      Один "шаг" выполнения данного действия                                      *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__step( const float & elapsed_since_last_call ) {
    
    if (( __started ) && ( ! __finished )) {
        
        
        // Управление скоростью - одно для всех фаз (действий).
        __control_of_speed( elapsed_since_last_call );
        // Управление угловым положением самолета.
        __control_of_angles( elapsed_since_last_call );
        
        // До перемещения - запоминаем предыдущее положение.
        auto front_wp = _get_front_wp();
        __previous_distance_to_front_wp = (int) _calculate_distance_to_wp( front_wp );
        // Прямолинейное перемещение самолета 
        __move_straight( elapsed_since_last_call );
        
        // "Внутренний шаг" - пересчет нужных для данного действия параметров.
        _internal_step( elapsed_since_last_call );
                
        __total_duration += elapsed_since_last_call;
    }
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                            Посчитать расстояние до "достаточно крутого поворота"                                 *
// *                                                                                                                  *
// ********************************************************************************************************************

double AircraftAbstractAction::_calculate_distance_to_turn() {
    
    auto location = _get_acf_location();
    for ( int i=0; i<_ptr_acf->_flight_plan.size(); i++ ) {
        auto wp = _ptr_acf->_flight_plan.at(i);
        double change = wp.incomming_heading - wp.outgoing_heading;
        if ( change >= 20.0 ) {
            return xenon::distance(location, wp.location);
        }
    }
    return 0.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Посчитать расстояние до точки, которая принадлежит ВПП                              *
// *                                                                                                                   *
// *********************************************************************************************************************

float AircraftAbstractAction::_calculate_distance_to_runway() {
    auto location = _get_acf_location();
    for ( int i=0; i<_ptr_acf->_flight_plan.size(); i++ ) {
        auto wp = _ptr_acf->_flight_plan.at(i);
        if ( wp.type == WAYPOINT_RUNWAY ) {
            return (float) xenon::distance( location, wp.location );
        }
    };
    return 0.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Окончание выполнения данного действия                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_finish() {
    __finished = true;
    _ptr_acf->_action_finished( this );
}
