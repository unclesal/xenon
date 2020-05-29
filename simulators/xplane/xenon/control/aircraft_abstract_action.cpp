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
        
        // Массив предыдущих расхождений по курсу для каждой фазы будет свой.
        for ( int i=0; i<PREVIOUS_ARRAY_SIZE; ++ i ) __previous_heading_delta[i] = 0.0;

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
    _params.speed_kts = meters_per_second_to_knots( _params.speed ); 
    
    // Вертикальная скорость и достижение ею целевого показателя.
    bool changed = false; // Оно здесь не нужно, но нужно в параметрах - ок.
    
    __control_of_one_value( 
        elapsed_since_last_call, _params.vertical_acceleration,
        _params.target_vertical_speed, _params.vertical_speed,
        changed
    );
    
    // Смещение самолета по вертикальной оси, если оно нужно.
    if ( _params.vertical_speed != 0.0 ) {
#ifdef INSIDE_XPLANE
        auto position = _ptr_acf->get_position();
        position.y += _params.vertical_speed * elapsed_since_last_call;
        _ptr_acf->set_position( position );
#else
        auto location = _ptr_acf->get_location();
        location.altitude += _params.vertical_speed * elapsed_since_last_call;
        _ptr_acf->set_location( location );
#endif
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
    if ( abs( _params.speed ) >= 0.8 ) {
        
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
// *                Подруливание на первую точку полетного плана по курсу (в "автомобильной" реализации).              *
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

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             "Подруливание" на курс - в авиационной реализации, не в колесной                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_head_bearing( const waypoint_t & wp ) {
    
    auto rotation = _get_acf_rotation();
    
    if ( wp.type == WAYPOINT_UNKNOWN ) {
        XPlane::log("ERROR: AircraftDoesFlying::__head_bearing(), type of front FP waypoint is UNKNOWN...");
        return;
    };
    
    auto location = _get_acf_location();
    auto heading = _get_acf_rotation().heading;
    auto bearing = xenon::bearing( location, wp.location );
    if (( bearing < 90.0 ) && ( heading > 270.0 )) bearing += 360;
    auto delta = bearing - heading;
    
    // Работа PID-регулятора, который устанавливает крен самолета. 
    // Сдвиг по курсу потом формируется - уже в зависимости от крена.
    // Особо не подбирался, оставил первый результат, зрительно 
    // более-менее похожий на правду.
    
    double P = 1.0;
    double D = 0.0; // Не понадобилось, и так хорошо.
    double I = 1.0;
    
    double ivalue = 0.0;    
    for ( int i=0; i<PREVIOUS_ARRAY_SIZE; ++ i ) {
        ivalue += __previous_heading_delta[i];
    };
    ivalue /= (double) PREVIOUS_ARRAY_SIZE;
    
    double dvalue = 0.0;
    for ( int i=0; i<PREVIOUS_ARRAY_SIZE - 1; ++i ) {
        dvalue += __previous_heading_delta[i] - __previous_heading_delta[i+1];
    };        
    
    double regulator_out = P * delta + D * dvalue + I * ivalue;
    
    if ( regulator_out >= 25.0 ) regulator_out = 25.0;
    if ( regulator_out <= -25.0 ) regulator_out = -25.0;
        
    _params.target_roll = regulator_out;
    delta < 0 ? _params.roll_acceleration = -3.0 : _params.roll_acceleration = 3.0;
        
    // Чтобы считать синусы-косинусы - надо иметь сдвинутый угол.
    // Повернутость угла влияет на знак, но он и так учитывается дальше.
    double radians = degrees_to_radians( rotation.roll - 90.0 );
    
    double dh = cos( radians );
    
    // Этот коэффициент не сильно важен. Важно, чтобы курс изменился.
    _params.target_heading = heading + dh * 10;
    
    // А этот коэффициент определяет скорость вращения в воздухе.
    // Подобран исходя из правдоподобности зрительного восприятия картинки.
    _params.heading_acceleration = 11.5 * dh;

    for (int i = PREVIOUS_ARRAY_SIZE - 2 ; i >= 0; -- i ) {
        __previous_heading_delta[i+1] = __previous_heading_delta[i];
    };
    __previous_heading_delta[0] = delta;    
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *      Автоматическое подруливание по высоте - через указанное время должна быть достигнута целевая высота.         *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_altitude_adjustment( const float & target_altitude, const float & time_to_achieve ) {
    
    auto location = _get_acf_location();
    auto acf_rotation = _get_acf_rotation();
    auto current_altitude = location.altitude;

    auto da = target_altitude - current_altitude;        
    
    // Сначала обнуляем параметры изменения высоты, т.к. пока еще непонятно,
    // в каком положении мы сейчас находимся, выше или ниже целевого значения.
    _params.pitch_acceleration = 0.0;
    _params.target_pitch = 0.0;
    
    _params.vertical_acceleration = 0.0;
    _params.target_vertical_speed = 0.0;
    
    // Вертикальная скорость выбирается таким образом, чтобы достигнуть нужной нам высоты прямо на 
    // точке привода. Несколько "не кошерно" в том плане, что вертикальная скорость может измениться 
    // рывком. Но видно этого не будет при любом раскладе событий, так что - вполне допустимо.
    
    _params.vertical_speed = 0.0;
    if ( time_to_achieve ) _params.vertical_speed = da / time_to_achieve;
    
    // Если ему просто взять и поставить некий градус, скажем, 5, то выглядит не 
    // реалистично. Угол тангажа надо ставить в зависимости от вертикальной скорости.
    
    float degrees = 1.0;
    if ( abs(_params.vertical_speed) > 7.0 ) degrees = 10.0;
    else if ( abs(_params.vertical_speed) > 5.0 ) degrees = 5.0;
    else if ( abs(_params.vertical_speed) > 2.0 ) degrees = 2.0;
    
    // Целевое значение тангажа.
    if ( da > 0.0 ) {
        // Мы находимся - ниже, надо подниматься.
        _params.target_pitch = degrees;
    } else if ( da < 0.0 ) {            
        // Мы находимся - выше, надо опускаться.            
        _params.target_pitch = -degrees;                        
    }
    
    // Изменение тангажа. А вот тангаж можно увидеть. 
    // Соответственно, резко изменяться он не может.
    if ( acf_rotation.pitch > _params.target_pitch ) _params.pitch_acceleration = -1.0f;
    else _params.pitch_acceleration = 1.0f;
    
//     XPlane::log(
//         "target=" + to_string( target_altitude )
//         + ", current=" + to_string( current_altitude )
//         + ", da=" + to_string( da )
//         + ", vs=" + to_string( _params.vertical_speed )
//         + ", pithc=" + to_string( acf_rotation.pitch )
//     );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *        Автоматическая подстройка скорости. Целевая скорость должна быть достигнута через указанное время          *
// *                                                                                                                   *
// *********************************************************************************************************************

void xenon::AircraftAbstractAction::_speed_adjustment( const float & target_speed, const float & time_to_achieve) {
    auto ds = target_speed - _params.speed;
    _params.target_speed = target_speed;
    _params.acceleration = 0.0;
    if ( time_to_achieve != 0.0 ) _params.acceleration = ds / time_to_achieve;
        
//         XPlane::log(
//             "Speed=" + to_string(_params.speed) + ", target=" + to_string( target_speed )
//             + ", ds=" + to_string( ds ) + ", achieve=" + to_string( time_to_achieve )
//             + ", accel=" + to_string( _params.acceleration )
//         );

}

