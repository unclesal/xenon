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
    
    _tug = 0.0;
    _acceleration = 0.0;
    _target_acceleration = 0.0;
    _speed = 0.0;
    _target_speed = 0.0;
    
    __total_duration = 0.0;
    __started = false;
    __finished = false;
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
        
        _internal_start();
        
    };
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                               Управление скоростью                                               *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__control_of_speed( float elapsed_since_last_call ) {
    
    if ( _tug != 0.0 ) {
        if ( abs(_acceleration) != abs( _target_acceleration )) {
            _acceleration += _tug * elapsed_since_last_call;
            if ( abs( _acceleration ) > abs( _target_acceleration ) ) _acceleration = _target_acceleration;
        }
    }
    
    if ( abs(_speed) != abs(_target_speed) ) {
        _speed += _acceleration * elapsed_since_last_call;
        if ( abs( _speed ) > abs( _target_speed ) ) _speed = _target_speed;
    }
    
    XPlane::log("Now acceleration=" + to_string( _acceleration ) + ", speed=" + to_string( _speed ) );
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                         Прямолинейное перемещение самолета                                       *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__move_straight( float elapsed_since_last_call ) {
    double distance = _speed * elapsed_since_last_call;
    __total_distance += distance;
    _ptr_acf->move( distance );
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                      Вернуть нулевую точку плана полета (front)                                  *
// *                                                                                                                  *
// ********************************************************************************************************************

waypoint_t & AircraftAbstractAction::_get_front_wp() {
    
    return _ptr_acf->_flight_plan.at(0);
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                       Нулевая точка плана полета (front) была достигнута, ее из FP нужно удалить.                *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::_front_wp_reached() {
    
    _ptr_acf->_flight_plan.pop_front();
    
};

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                      Один "шаг" выполнения данного действия                                      *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__step( float elapsed_since_last_call ) {
    
    if (( __started ) && ( ! __finished )) {
        
        // Управление скоростью - одно для всех фаз (действий).
        __control_of_speed( elapsed_since_last_call );
        // Прямолинейное перемещение самолета 
        __move_straight( elapsed_since_last_call );
        
        // "Внутренний шаг" - пересчет координат.
        _internal_step( elapsed_since_last_call );
        
        __total_duration += elapsed_since_last_call;
    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Окончание выполнения данного действия                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_finish() {
    __finished = true;
    if ( _ptr_acf ) _ptr_acf->_action_finished( this );
}
