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
): AbstractAircrafter( ptr_acf ) 

{
    _edge_d = edge_d;
        
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

void AircraftAbstractAction::__control_of_speeds( const float & elapsed_since_last_call ) {
    
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
    
    // После вычисления скорости происходит ее запоминание в морских миль в час
    _params.speed_kph = meters_per_second_to_knodes_per_hour( _params.speed ); 
    
    // Вертикальная скорость и достижение ею целевого показателя.
    bool changed = false; // Оно здесь не нужно, но нужно в параметрах - ок.
    
    __control_of_one_value( 
            elapsed_since_last_call, _params.vertical_acceleration, 
            _params.target_vertical_speed, _params.vertical_speed, 
            changed         
    );
    
    if ( _params.vertical_speed != 0.0 ) {
        auto position = _ptr_acf->get_position();
        position.y += _params.vertical_speed * elapsed_since_last_call;
        _ptr_acf->set_position( position );
    }
    
    // После вычисления вертикальной скорости запоминаем значение в футах в минуту.
    _params.vertical_speed_fpm = meters_per_seconds_to_feet_per_min( _params.vertical_speed );
    
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
// *                                 Управление одной величиной с учетом конечной точки                               *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__control_of_one_value( 
    const float & elapsed_since_last_call, float & acceleration, const float & endpoint, float & controlled_value, bool & changed
) {
    float av = acceleration * elapsed_since_last_call;
    if ( av != 0.0 ) {
        controlled_value += av;
        changed = true;
        
        if ( acceleration < 0 ) {
            // Ускорение - отрицательное, значение может стать - меньше заданного.
            if ( controlled_value < endpoint ) {
                controlled_value = endpoint;
                acceleration = 0.0;
            }
        } else if ( acceleration  > 0 ) {
            // Ускорение - положительное, значение может стать - больше заданного.
            if ( controlled_value > endpoint ) {
                controlled_value = endpoint;
                acceleration = 0.0;
            };
        }
        
    }
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                       Управление угловым положением самолета                                     *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__control_of_angles( const float & elapsed_since_last_call ) {
                
    // Углы подравниваем только в том случае, если есть хоть какая-то скорость.
    // Чтобы избежать вращения самолета на месте.
    // if ( abs( _params.speed ) >= 0.8 ) {
        
        auto rotation = _ptr_acf->get_rotation();
        bool changed = false;
        
        // У абстрактного действия нет контроля за конечной точкой положения по курсу.
        // Это позволяет внутри реального действия осуществлять автоматическое подруливание.
        
        if ( _params.heading_acceleration != 0.0 ) {                
            double heading = rotation.heading;
            heading += _params.heading_acceleration * elapsed_since_last_call;
            normalize_degrees( heading );
            rotation.heading = heading;
            changed = true;            
        }
        
        // Остальные две в нормализации не нуждаются. Но зато у них есть
        // контроль конечных точек. С таким расчетом, чтобы один раз поставить
        // ускорение - и больше не проверять его, самолет сам со временем
        // встанет в нужное угловое положение.
        __control_of_one_value( 
                elapsed_since_last_call, _params.pitch_acceleration, 
                _params.target_pitch, rotation.pitch, 
                changed 
        );
        __control_of_one_value( 
                elapsed_since_last_call, _params.roll_acceleration, 
                _params.target_roll, rotation.roll, 
                changed 
        );
                
        if ( changed ) _ptr_acf->set_rotation( rotation );
    // }
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                      Один "шаг" выполнения данного действия                                      *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__step( const float & elapsed_since_last_call ) {
    
    if (( __started ) && ( ! __finished )) {
        
        
        // Управление скоростью - одно для всех фаз (действий).
        __control_of_speeds( elapsed_since_last_call );
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

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Подруливание на первую точку полетного плана по курсу.                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_head_steering( float elapsed_since_last_call, double kp ) {

    if ( _is_flight_plan_empty() ) {
        XPlane::log("ERROR: AircraftAbstractAction::_head_steering, but FP is empty");
        return;
    };
    
    auto wp = _get_front_wp();
    auto bearing = xenon::bearing( _get_acf_location(), wp.location );
    auto heading = _get_acf_rotation().heading;
    auto delta = bearing - heading;        
    _params.target_heading = bearing;
    _params.heading_acceleration = kp * delta * elapsed_since_last_call;

}
