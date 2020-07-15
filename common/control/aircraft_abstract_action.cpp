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
    _ptr_acf = ptr_acf;
    _edge_d = edge_d;
        
    _total_duration = 0.0;

}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                              Старт данного действия                                              *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__start() {
    
    
    // Установка тех переменных, которые суммируются в ходе выполнения данного действия. Одно
    // и то же действие может быть вызвано - неоднократно. Поэтому при старте их надо обнулять.
    
    _total_duration = 0.0;
    _total_distance = 0.0;
    
    auto wp = _ptr_acf->flight_plan.get(0);
    auto location = _ptr_acf->get_location();
    auto distance_to_front = (int) xenon::distance2d( location, wp.location );
            
    for ( int i=0; i<PREVIOUS_ARRAY_SIZE; ++ i ) {
        
        // Массив предыдущих дистанций - в начале действия там все
        // дистанции должны быть равны текущей. Иначе точка сразу же
        // может начать удаляться.
        // __previous_distance_to_front_wp[ i ] = (int) distance_to_front;
        
        // Массив предыдущих расхождений по курсу для каждой фазы будет свой.
        __previous_heading_delta[ i ] = 0.0;
        
    }
    
    _internal_start();
            
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                               Управление скоростью                                               *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__control_of_speeds( const float & elapsed_since_last_call ) {
    
//     if ( _params.tug != 0.0 ) {
//         if ( abs( _params.acceleration ) != abs( _params.target_acceleration )) {
//             _params.acceleration += _params.tug * elapsed_since_last_call;
//             if ( _params.tug < 0.0 ) {
//                 "Рывок" - ниже нуля. Ускорение может стать - меньше заданного.
//                 if ( _params.acceleration < _params.target_acceleration ) 
//                     _params.acceleration = _params.target_acceleration;
//             } else {
//                 "Рывок" - больше нуля. Ускорение может стать - больше заданного.
//                 if ( _params.acceleration > _params.target_acceleration ) 
//                     _params.acceleration = _params.target_acceleration;
//             };
//         }
//     }
    
    if ( ( abs( _ptr_acf->vcl_condition.speed ) != abs( _ptr_acf->vcl_condition.target_speed ) ) && ( _ptr_acf->vcl_condition.acceleration != 0.0 ) ) {
        _ptr_acf->vcl_condition.speed += _ptr_acf->vcl_condition.acceleration * elapsed_since_last_call;
        
        if ( _ptr_acf->vcl_condition.acceleration < 0.0 ) {
            // Ускорение - меньше нуля. Скорость может стать - ниже заданной.
            if ( _ptr_acf->vcl_condition.speed < _ptr_acf->vcl_condition.target_speed ) _ptr_acf->vcl_condition.speed = _ptr_acf->vcl_condition.target_speed;
        } else {
            // Ускорение - больше нуля. Скорость может стать - выше заданной.
            if ( _ptr_acf->vcl_condition.speed > _ptr_acf->vcl_condition.target_speed ) _ptr_acf->vcl_condition.speed = _ptr_acf->vcl_condition.target_speed;
        }        
    }
    
    // После вычисления скорости происходит ее запоминание в морских миль в час
    _ptr_acf->vcl_condition.speed_kts = meters_per_second_to_knots( _ptr_acf->vcl_condition.speed ); 
    
    // Вертикальная скорость и достижение ею целевого показателя.
    bool changed = false; // Оно здесь не нужно, но нужно в параметрах - ок.
    
    __control_of_one_value( 
        elapsed_since_last_call, _ptr_acf->acf_condition.vertical_acceleration,
        _ptr_acf->acf_condition.target_vertical_speed, _ptr_acf->acf_condition.vertical_speed,
        changed
    );
    
    // Смещение самолета по вертикальной оси, если оно нужно.
    if ( _ptr_acf->acf_condition.vertical_speed != 0.0 ) {
#ifdef INSIDE_XPLANE
        auto position = _ptr_acf->get_position();
        position.y += _ptr_acf->acf_condition.vertical_speed * elapsed_since_last_call;
        _ptr_acf->set_position( position );
#else
        auto location = _ptr_acf->get_location();
        location.altitude += _ptr_acf->acf_condition.vertical_speed * elapsed_since_last_call;
        _ptr_acf->set_location( location );
#endif
    }
    
    // После вычисления вертикальной скорости запоминаем значение в футах в минуту.
    _ptr_acf->acf_condition.vertical_speed_fpm = meters_per_seconds_to_feet_per_min( _ptr_acf->acf_condition.vertical_speed );
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                         Прямолинейное перемещение самолета                                       *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftAbstractAction::__move_straight( const float & elapsed_since_last_call ) {
    
    double distance = _ptr_acf->vcl_condition.speed * elapsed_since_last_call;
    _ptr_acf->move( distance );
    
    // Пройденная дистанция в любом случае увеличивается, даже если
    // самолет при этом двигается назад (при его выталкивании).

    _total_distance += abs(distance);
    
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
    if ( abs( _ptr_acf->vcl_condition.speed ) >= 0.8 ) {
        
        auto rotation = _ptr_acf->get_rotation();
        bool changed = false;
        
        // У абстрактного действия нет контроля за конечной точкой положения по курсу.
        // Это позволяет внутри реального действия осуществлять автоматическое подруливание.
        
        if ( _ptr_acf->vcl_condition.heading_acceleration != 0.0 ) {
            double heading = rotation.heading;
            heading += _ptr_acf->vcl_condition.heading_acceleration * elapsed_since_last_call;
            normalize_degrees( heading );
            rotation.heading = heading;
            changed = true;            
        }
        
        // Остальные две в нормализации не нуждаются. Но зато у них есть
        // контроль конечных точек. С таким расчетом, чтобы один раз поставить
        // ускорение - и больше не проверять его, самолет сам со временем
        // встанет в нужное угловое положение.
        __control_of_one_value( 
                elapsed_since_last_call, _ptr_acf->acf_condition.pitch_acceleration, 
                _ptr_acf->acf_condition.target_pitch, rotation.pitch, 
                changed 
        );
        __control_of_one_value( 
                elapsed_since_last_call, _ptr_acf->acf_condition.roll_acceleration, 
                _ptr_acf->acf_condition.target_roll, rotation.roll, 
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
            
        
    // Управление скоростью - одно для всех фаз (действий).
    __control_of_speeds( elapsed_since_last_call );
    // Управление угловым положением самолета.
    __control_of_angles( elapsed_since_last_call );
    
    // До перемещения - запоминаем предыдущее положение.
    auto front_wp = _ptr_acf->flight_plan.get(0);
        
//         for ( int i = PREVIOUS_ARRAY_SIZE - 2; i>=0; -- i ) {
//             __previous_distance_to_front_wp[i + 1] = __previous_distance_to_front_wp[i];
//         }
//               
//         __previous_distance_to_front_wp[0] = (int) _calculate_distance_to_wp( front_wp );
        
    // Прямолинейное перемещение самолета 
    __move_straight( elapsed_since_last_call );
    
    // "Внутренний шаг" - пересчет нужных для данного действия параметров.
    _internal_step( elapsed_since_last_call );
            
    _total_duration += elapsed_since_last_call;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                Подруливание на первую точку полетного плана по курсу (в "автомобильной" реализации).              *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_head_steering( float elapsed_since_last_call, double kp ) {

    if ( _ptr_acf->flight_plan.is_empty() ) {
        Logger::log(_ptr_acf->vcl_condition.agent_name + ": AircraftAbstractAction::_head_steering, but FP is empty");
        return;
    };
    
    auto wp = _ptr_acf->flight_plan.get(0);
    auto bearing = xenon::bearing( _ptr_acf->get_location(), wp.location );    
    auto delta = _get_delta_bearing( wp );

    _ptr_acf->vcl_condition.target_heading = bearing;
    _ptr_acf->vcl_condition.heading_acceleration = kp * delta * elapsed_since_last_call;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             "Подруливание" на курс - в авиационной реализации, не в колесной                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_head_bearing( const waypoint_t & wp ) {
    
    auto rotation = _ptr_acf->get_rotation();
    
    if ( wp.type == WAYPOINT_UNKNOWN ) {
        Logger::log("ERROR: AircraftDoesFlying::__head_bearing(), type of front FP waypoint is UNKNOWN...");
        return;
    };
    
    auto heading = rotation.heading;
    
//     auto location = _get_acf_location();    
//     auto bearing = xenon::bearing( location, wp.location );
//     if (( bearing < 90.0 ) && ( heading > 270.0 )) bearing += 360;
//     auto delta = bearing - heading;
    
    auto delta = _get_delta_bearing( wp );
    
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
        
    _ptr_acf->acf_condition.target_roll = regulator_out;
    delta < 0 ? _ptr_acf->acf_condition.roll_acceleration = -3.0 : _ptr_acf->acf_condition.roll_acceleration = 3.0;
        
    // Чтобы считать синусы-косинусы - надо иметь сдвинутый угол.
    // Повернутость угла влияет на знак, но он и так учитывается дальше.
    double radians = degrees_to_radians( rotation.roll - 90.0 );
    
    double dh = cos( radians );
    
    // Этот коэффициент не сильно важен. Важно, чтобы курс изменился.
    _ptr_acf->vcl_condition.target_heading = heading + dh * 10;
    
    // А этот коэффициент определяет скорость вращения в воздухе.
    // Подобран исходя из правдоподобности зрительного восприятия картинки.
    _ptr_acf->vcl_condition.heading_acceleration = 11.5 * dh;

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
    
    auto location = _ptr_acf->get_location();
    auto acf_rotation = _ptr_acf->get_rotation();
    auto current_altitude = location.altitude;

    auto da = target_altitude - current_altitude;        
    
    // Сначала обнуляем параметры изменения высоты, т.к. пока еще непонятно,
    // в каком положении мы сейчас находимся, выше или ниже целевого значения.
    _ptr_acf->acf_condition.pitch_acceleration = 0.0;
    _ptr_acf->acf_condition.target_pitch = 0.0;
    
    _ptr_acf->acf_condition.vertical_acceleration = 0.0;
    _ptr_acf->acf_condition.target_vertical_speed = 0.0;
    
    // Вертикальная скорость выбирается таким образом, чтобы достигнуть нужной нам высоты прямо на 
    // точке привода. Несколько "не кошерно" в том плане, что вертикальная скорость может измениться 
    // рывком. Но видно этого не будет при любом раскладе событий, так что - вполне допустимо.
    
    _ptr_acf->acf_condition.vertical_speed = 0.0;
    if ( time_to_achieve ) _ptr_acf->acf_condition.vertical_speed = da / time_to_achieve;
    
    // Если ему просто взять и поставить некий градус, скажем, 5, то выглядит не 
    // реалистично. Угол тангажа надо ставить в зависимости от вертикальной скорости.
    
    float degrees = 1.0;
    if ( abs(_ptr_acf->acf_condition.vertical_speed) > 7.0 ) degrees = 4.0;
    else if ( abs(_ptr_acf->acf_condition.vertical_speed) > 5.0 ) degrees = 3.0;
    else if ( abs(_ptr_acf->acf_condition.vertical_speed) > 2.0 ) degrees = 2.0;
    
    // Целевое значение тангажа.
    if ( da > 0.0 ) {
        // Мы находимся - ниже, надо подниматься.
        _ptr_acf->acf_condition.target_pitch = degrees;
    } else if ( da < 0.0 ) {            
        // Мы находимся - выше, надо опускаться.            
        _ptr_acf->acf_condition.target_pitch = -degrees;                        
    }
    
    // Изменение тангажа. А вот тангаж можно увидеть. 
    // Соответственно, резко изменяться он не может.
    if ( acf_rotation.pitch > _ptr_acf->acf_condition.target_pitch ) _ptr_acf->acf_condition.pitch_acceleration = -1.0f;
    else _ptr_acf->acf_condition.pitch_acceleration = 1.0f;
    
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

void AircraftAbstractAction::_speed_adjustment( const float & target_speed, const float & time_to_achieve) {
    auto ds = target_speed - _ptr_acf->vcl_condition.speed;
    _ptr_acf->vcl_condition.target_speed = target_speed;
    _ptr_acf->vcl_condition.acceleration = 0.0;
    if ( time_to_achieve != 0.0 ) _ptr_acf->vcl_condition.acceleration = ds / time_to_achieve;
        
//         XPlane::log(
//             "Speed=" + to_string(_params.speed) + ", target=" + to_string( target_speed )
//             + ", ds=" + to_string( ds ) + ", achieve=" + to_string( time_to_achieve )
//             + ", accel=" + to_string( _params.acceleration )
//         );

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                     Получить разницу между текущим курсом самолета и азимутом на точку                            *
// *                                                                                                                   *
// *********************************************************************************************************************

double AircraftAbstractAction::_get_delta_bearing( const waypoint_t & wp ) {
    
    auto bearing = xenon::bearing( _ptr_acf->get_location(), wp.location );    
    auto heading = _ptr_acf->get_rotation().heading;
    auto delta = bearing - heading;
    
    /*
    if ( _ptr_acf->vcl_condition.current_action == ACF_DOES_TAKE_OFF ) {
        Logger::log(
            _ptr_acf->vcl_condition.agent_name + " before transformation: " 
            + wp.name + ", type=" + waypoint_to_string( wp.type )
            + ", action=" + action_to_string( wp.action_to_achieve )
            + ", distance=" + to_string( xenon::distance2d(_ptr_acf->get_location(), wp.location))
            + ", heading=" + to_string( heading )
            + ", bearing=" + to_string( bearing )
            + ", delta=" + to_string( delta )
        );    
    }
    */
    
    if ( abs(delta) >= 180.0 ) {

        auto delta2 = 0.0;
        if (( heading >= 180.0 ) && ( bearing <= 180 ))
            delta2 = delta + 360.0;
        else
            delta2 = delta - 360.0;

        if ( abs( delta2 ) < abs( delta )) delta = delta2;
    }

    /*
    if ( _ptr_acf->vcl_condition.current_action == ACF_DOES_TAKE_OFF ) {
        Logger::log(
            _ptr_acf->vcl_condition.agent_name + " after transformation: " 
            + wp.name + ", type=" + waypoint_to_string( wp.type )
            + ", action=" + action_to_string( wp.action_to_achieve )
            + ", distance=" + to_string( xenon::distance2d(_ptr_acf->get_location(), wp.location))
            + ", heading=" + to_string( heading )
            + ", bearing=" + to_string( bearing )
            + ", delta=" + to_string( delta )
        );    
    }
    */
    
    return delta;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *          Получить разницу между текущим курсом самолета и целевым курсом на нулевой точке полетного плана         *
// *                                                                                                                   *
// *********************************************************************************************************************

double AircraftAbstractAction::_get_delta_to_target_heading( const waypoint_t & wp ) {
    
    // Текущий курс самолета.
    double heading = _ptr_acf->get_rotation().heading;
    xenon::normalize_degrees( heading );
    // Целевой курс, как нам надо встать.
    double target_heading = wp.outgoing_heading;
    xenon::normalize_degrees( target_heading );

    double delta_heading = target_heading - heading;
    // Здесь нормализация - не выполняется. Потому что она нужна не всегда.
    return delta_heading;
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Применить торможение при рулении                                           *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_taxi_breaking( const float & to_speed, const float & for_seconds ) {
    auto cur_speed = _ptr_acf->vcl_condition.speed;
    if (( cur_speed == 0.0 ) || ( for_seconds == 0.0 )) return;
    auto ds = to_speed - cur_speed;
    _ptr_acf->vcl_condition.target_speed = to_speed;
    _ptr_acf->vcl_condition.acceleration = ds / for_seconds;
    
//     XPlane::log(
//         "Cur speed=" + to_string( cur_speed ) + ", to speed=" + to_string( to_speed )
//         + ", ds=" + to_string( ds ) + ", accel=" + to_string( _ptr_acf->vcl_condition.acceleration)
//     );
}


// *********************************************************************************************************************
// *                                                                                                                   *
// *       Вычисление точки поворота при рулении. И если она достигнута, то установка параметров этого поворота.       *
// *                                                                                                                   *
// *********************************************************************************************************************

bool AircraftAbstractAction::_taxi_turn_started( const waypoint_t & destination ) {
    
    // Точка, где мы сейчас находимся.
    auto me = _ptr_acf->get_location();
    
    // Точка, сдвинутая метров на сколько-нибудь от destination для описания сегмента.
    auto wp1 = xenon::shift( destination.location, 25.0, destination.outgoing_heading );
    
    // Разница в курсах (для определения направления поворота, вправо или влево)
    double delta_heading = _get_delta_to_target_heading( destination );
    // Нормализация, потому что сейчас будем определять сторону поворота.
    normalize_degrees( delta_heading );

    auto taxi_radius = TAXI_TURN_RADIUS;
    if ( delta_heading > 180.0 ) taxi_radius = - taxi_radius;
    
    // Точка, сдвинутая от нашей текущей позиции в сторону поворота на радиус 
    // поворота. Эта точка находится на линии, где будет центр разворота.
    auto me_shifted = xenon::shift( me, taxi_radius, _ptr_acf->vcl_condition.rotation.heading + 90.0 );
    
    auto wp_shifted = xenon::shift( destination.location, taxi_radius, destination.outgoing_heading - 90.0 );
    auto wp1_shifted = xenon::shift( wp1, taxi_radius, destination.outgoing_heading - 90.0 );
        
    auto dist_shifted_me_to_segment = xenon::distance_to_segment( me_shifted, wp_shifted, wp1_shifted );
//    XPlane::log("dis=" + to_string( dist_shifted_me_to_segment ));
    
    // Место разворота, похоже, зависит от того, вперед самолет 
    // едет или назад. И похоже что это - длина самолета.

    float threshold = 2.5;
    if ( _ptr_acf->vcl_condition.speed > 0 ) threshold += _ptr_acf->parameters().length / 2.0;
        
    if ( dist_shifted_me_to_segment <= threshold ) {
        // Длина дуги, которую нам надо пройти.
        auto len = TAXI_TURN_RADIUS * degrees_to_radians( delta_heading );
        // Время, за которое мы ее пройдем.
        auto dt = len / _ptr_acf->vcl_condition.speed;
        // Изменение по курсу, градусов в секунду.
        auto delta_h = delta_heading / dt;
        if ( _ptr_acf->vcl_condition.speed < 0.0 ) delta_h = - delta_h;
        
        delta_heading < 180.0 ?
            _ptr_acf->vcl_condition.heading_acceleration = delta_h 
            : _ptr_acf->vcl_condition.heading_acceleration = - delta_h;
            
        // Самолету надо поставить целевой курс, чтобы он начал поворачивать.
        _ptr_acf->vcl_condition.target_heading = destination.outgoing_heading;
        
//         XPlane::log(
//             "cur heading=" + to_string( _ptr_acf->vcl_condition.rotation.heading)
//             + ", wanted=" + to_string( destination.outgoing_heading )
//             + ", delta=" + to_string( delta_heading )
//             + ", accel=" + to_string( _ptr_acf->vcl_condition.heading_acceleration )
//         );
        
        return true;
    }

    return false;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                             Управление положением закрылков в зависимости от скорости                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftAbstractAction::_control_of_flaps() {
    
    auto acf_params = _ptr_acf->parameters();
    
    float current_flaps = _ptr_acf->acf_condition.flaps_position;
    
    if (
        ( _ptr_acf->vcl_condition.speed >= xenon::knots_to_merets_per_second( acf_params.flaps_take_off_speed ))
        && ( _ptr_acf->vcl_condition.acceleration > 0.0 )
        && ( current_flaps != 0.0 )
    ) {
        _ptr_acf->set_flaps_position(0.0);
        Logger::log("FLY: laps to 0");
    }
    
    if ( 
        ( _ptr_acf->vcl_condition.speed )
        && ( _ptr_acf->vcl_condition.speed <= xenon::knots_to_merets_per_second( acf_params.flaps_take_off_speed))
        && ( _ptr_acf->vcl_condition.speed > xenon::knots_to_merets_per_second( acf_params.flaps_landing_speed ))
        && ( _ptr_acf->vcl_condition.acceleration < 0.0 )
        && ( current_flaps != acf_params.flaps_take_off_position )
    ) {
        _ptr_acf->set_flaps_position( acf_params.flaps_take_off_position );
        Logger::log("FLY: flaps to TO");
    };
    
    // Если скорость еще снизилась, то закрылки выпускаем в посадочное положение 
        
    if (
        ( _ptr_acf->vcl_condition.speed )
        && ( _ptr_acf->vcl_condition.speed <= xenon::knots_to_merets_per_second( acf_params.flaps_landing_speed ))
        && ( _ptr_acf->vcl_condition.acceleration < 0.0 )
        && ( current_flaps != 1.0 )
    ) {
        Logger::log("Flaps to LAND position, was " + to_string(current_flaps));
        _ptr_acf->set_flaps_position( 1.0 );
        _ptr_acf->set_gear_down( true );
    }     
    
    if ( _ptr_acf->acf_condition.flaps_position == 1.0 ) {
        
        // Если закрылки выпущены в посадочное положение, то считаем, что самолет летит 
        // уже достаточно медленно. Поэтому выравнивается на положительный тангаж.
        // При этом будут перекрываться установки по тангажу _altitude_adjustment.
        
        _ptr_acf->acf_condition.target_pitch = 3.5;
        
        // Нос пошел вверх. 
        
        if ( _ptr_acf->vcl_condition.rotation.pitch > _ptr_acf->acf_condition.target_pitch ) 
            _ptr_acf->acf_condition.pitch_acceleration = -0.3f;
        else
            _ptr_acf->acf_condition.pitch_acceleration = 0.3f;
    }


};
