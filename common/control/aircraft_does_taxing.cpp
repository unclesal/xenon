// *********************************************************************************************************************
// *                                                Самолет выполняет руление                                          *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 18 may 2020 at 13:57 *
// *********************************************************************************************************************

#include "aircraft_does_taxing.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesTaxing::AircraftDoesTaxing(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction( ptr_acf, edge_d )
{
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Перекрытая функция старта действия                                      *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesTaxing::_internal_start() {
    
    _ptr_acf->vcl_condition.is_clamped_to_ground = true;
    _ptr_acf->set_taxi_lites( true );
    _ptr_acf->set_landing_lites( true );
    _ptr_acf->set_beacon_lites( true );
    
    __from_runway_location = location_t();
    auto front_wp = _ptr_acf->flight_plan.get(0);
    if (
        ( _ptr_acf->vcl_condition.current_state == ACF_STATE_LANDED )
        && ( front_wp.type == WAYPOINT_RUNWAY )
    ) {
        __from_runway_location = front_wp.location;
    }
            
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *               Выбираем скорость в зависимости от расстояния до нулевой точки полетного плана                     *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftDoesTaxing::__choose_speed() {
    
    waypoint_t turned_wp;
    auto distance_to_turn = _ptr_acf->flight_plan.distance_to_turn( _ptr_acf->get_location(), turned_wp );
    // XPlane::log("Distance to turn=" + to_string( distance_to_turn ));
        
    if ( distance_to_turn >= 100.0 ) {                
        
        if ( _ptr_acf->vcl_condition.target_speed != TAXI_NORMAL_SPEED ) {
            
            // Logger::log("set TAXI_NORMAL_SPEED, distance=" + to_string( distance_to_turn ) + ", target=" + to_string(_ptr_acf->vcl_condition.target_speed));
        
            _ptr_acf->vcl_condition.acceleration = TAXI_NORMAL_ACCELERATION;
            _ptr_acf->vcl_condition.target_speed = TAXI_NORMAL_SPEED;
        }
        
    } else {
        
        // Если нам уже скоро поворачивать.        
        
        if ( _ptr_acf->vcl_condition.target_speed != TAXI_SLOW_SPEED ) {
            
            float time_to_reach = distance_to_turn / _ptr_acf->vcl_condition.speed;
            if ( time_to_reach <= 10.0 ) {
                // До точки поворота осталось меньше скольки-нибудь секунд - тормозим.
                // Logger::log("breaking to TAXI_SLOW_SPEED for 10 sec");
                _taxi_breaking( TAXI_SLOW_SPEED, 10.0);

            } else {
            
                if ( _ptr_acf->vcl_condition.speed > TAXI_SLOW_SPEED ) {                
                    // Скорость - высокая. Тормозим. 
                    // Logger::log("down to TAXI_SLOW_SPEED");
                    _taxi_breaking( TAXI_SLOW_SPEED, 3.0 );                    
                } else if ( _ptr_acf->vcl_condition.speed < TAXI_SLOW_SPEED ) {                    
                    // Текущая скорость низкая, можно подразогнаться до TAXI_SLOW_SPEED
                    // Logger::log("up to TAXI_SLOW_SPEED");
                    _ptr_acf->vcl_condition.acceleration = TAXI_SLOW_ACCELERATION;
                    _ptr_acf->vcl_condition.target_speed = TAXI_SLOW_SPEED;
                }
            }
        }                
                
    };    
    
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                    Отрабатываем подход к предварительному старту                                 *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftDoesTaxing::__become_to_hp( waypoint_t & front_wp ) {

    // Если в FP нет ВПП, то будет ровно 0. И здесь намеренно берется
    // не первая точка из полетного плана, т.к. между RWY и нами
    // могут быть еще точки руления, которые будут проигнорированы.

    auto here = _ptr_acf->get_location();
    auto distance_to_rwy = _ptr_acf->flight_plan.distance_to_runway( here );
    if (( distance_to_rwy > 0.0 ) && ( distance_to_rwy <= 200.0 ) ) {

        // подходим к HP.

        if ( _ptr_acf->vcl_condition.speed > 0.0 ) {

            // Дистанция до точки остановки перед HP.
            float dtr = distance_to_rwy - 120.0;

            float time_to_reach = dtr / _ptr_acf->vcl_condition.speed;
            if ( time_to_reach <= 5.0 ) {
                _taxi_breaking( 0.0, 5.0 );
            }
        }

        if (( _ptr_acf->vcl_condition.target_speed != 0.0 ) && ( _ptr_acf->vcl_condition.target_speed != TAXI_SLOW_SPEED )) {

            auto ds = TAXI_SLOW_SPEED - _ptr_acf->vcl_condition.speed;
            _ptr_acf->vcl_condition.acceleration = ds / 5.0;
            _ptr_acf->vcl_condition.target_speed = TAXI_SLOW_SPEED;
            
//             Logger::log(
//                 "Go to HP speed=" + to_string(_ptr_acf->vcl_condition.speed)
//                 + ", acceleration=" + to_string( _ptr_acf->vcl_condition.acceleration )
//             );

        }

        if ( abs(_ptr_acf->vcl_condition.speed) <= 0.5 ) {
            
//             Logger::log("Full stop on HP. Distance=" + to_string(distance_to_rwy));
//             Logger::log(
//                 "Lat=" + to_string( _get_acf_location().latitude )
//                 + ", lon=" + to_string( _get_acf_location().longitude )
//                 + ", heading=" + to_string( _get_acf_rotation().heading )
//             );

            _ptr_acf->vcl_condition.speed = 0.0;
            _ptr_acf->vcl_condition.acceleration = 0.0;
            _ptr_acf->vcl_condition.heading_acceleration = 0.0;
            // Действие было полностью выполнено, выходим.

            while ( front_wp.type == WAYPOINT_TAXING ) {
                _ptr_acf->flight_plan.pop_front();
                front_wp = _ptr_acf->flight_plan.get(0);
            }
            _finish();
            return;
        }

    } else {

        // Мы пока что достаточно далеко от HP.
        // Осуществляем обычное руление.

        __choose_speed();
    }

}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                               Перекрытая функция "шага"                                          *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftDoesTaxing::_internal_step( const float & elapsed_since_last_call ) {
    
    auto front_wp = _ptr_acf->flight_plan.get(0);

//    Logger::log(
//        "Front=" + front_wp.name
//        + ", dis=" + to_string( xenon::distance2d( _get_acf_location(), front_wp.location))
//    );

    _head_steering( elapsed_since_last_call, 16.0 );
    
    if ( _ptr_acf->vcl_condition.current_state != ACF_STATE_LANDED )
        __become_to_hp( front_wp );
    else
        __choose_speed();
    
    double distance = xenon::distance2d( _ptr_acf->get_location(), front_wp.location );
    
    if ( 
        ( _ptr_acf->vcl_condition.current_state == ACF_STATE_LANDED ) // то есть мы только что приземлились
        && ( __from_runway_location.latitude != 0.0 ) // то есть точка была чем-то заполнена
        && ( __from_runway_location.longitude != 0.0 )
        && ( front_wp.type != WAYPOINT_RUNWAY ) // то есть мы и правда слезли уже со взлетки
    ) {
        // Если мы сели, то пробуем уйти в состояние "освободил ВПП".
        auto disposal = xenon::distance2d( _ptr_acf->get_location(), __from_runway_location );
        if ( disposal >= 50 ) {
            _finish();
            return;
        }
    }
    
    if ( distance < 32.0 ) {
        
        auto reached_wp_type = front_wp.type;
        _ptr_acf->flight_plan.pop_front();
        
        // Если точка, которая только что была достигнута, это 
        // уход с ВПП - то переходим в состояние ухода с ВПП, т.к. 
        // нужно сообщить всем остальным агентам, что ВПП свободна.        
        
        if (
            ( reached_wp_type == WAYPOINT_RUNWAY_LEAVED ) 
            && ( _ptr_acf->vcl_condition.current_state == ACF_STATE_LANDED )
        ) {
            _finish();
            return;
        };
        
        // Следующая точка полетного плана.
        
        front_wp = _ptr_acf->flight_plan.get(0);
        if ( front_wp.type == WAYPOINT_PARKING ) {
            
            // Если следующая точка уже парковка, то просто выходим.
            // Скорости не корректируем, это сделает действие парковки.
            
            _finish();
            return;
        }
    }

}
