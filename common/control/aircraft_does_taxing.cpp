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
    
    if ( _ptr_acf->flight_plan.is_empty() ) _finish();
    
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
    
    // Logger::log("Distance to turn=" + to_string( distance_to_turn ));
        
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

            // Logger::log( "time_to_reach: " + to_string( time_to_reach ) );
            
            if ( time_to_reach <= 10.0 ) {
                // До точки поворота осталось меньше скольки-нибудь секунд - тормозим.
                // Logger::log("breaking to TAXI_SLOW_SPEED for " + to_string( time_to_reach - 1 ) + " sec");

                _taxi_breaking( TAXI_SLOW_SPEED, time_to_reach - 1 );

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
// *                                               Перекрытая функция "шага"                                          *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftDoesTaxing::_internal_step( const float & elapsed_since_last_call ) {
    
    if ( _ptr_acf->flight_plan.is_empty() ) _finish();
    
    auto front_wp = _ptr_acf->flight_plan.get(0);

//    Logger::log(
//        "Front=" + front_wp.name
//        + ", type=" + waypoint_to_string( front_wp.type ) + ", action=" + action_to_string( front_wp.action_to_achieve )
//        + ", dis=" + to_string( xenon::distance2d( _ptr_acf->get_location(), front_wp.location))
//    );

    _head_steering( elapsed_since_last_call, 25.0 );

    __choose_speed();
    
    double distance = xenon::distance2d( _ptr_acf->get_location(), front_wp.location );
    
    if ( front_wp.type == WAYPOINT_HP ) {
        
        if ( distance <= 70 && _ptr_acf->vcl_condition.target_speed ) {
            // Начинаем торможение 
            if ( _ptr_acf->vcl_condition.speed ) {
                auto time = distance / _ptr_acf->vcl_condition.speed;            
                _ptr_acf->vcl_condition.acceleration = - _ptr_acf->vcl_condition.speed / time;
                _ptr_acf->vcl_condition.target_speed = 0.0;
            };
        };
        
        if ( _ptr_acf->vcl_condition.speed <= 0.2 ) {
            _ptr_acf->flight_plan.pop_front();
            _finish();
            return;
        };
        
    };
    
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
    
    bool before_parking = (( _ptr_acf->flight_plan.size() >= 2 ) && ( _ptr_acf->flight_plan.get(1).type == WAYPOINT_PARKING ));
    double distance_threshold = 32;
    if ( before_parking ) distance_threshold = 20.0;

    if ( distance < distance_threshold) {
        
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
        
        if ( reached_wp_type == WAYPOINT_HP ) {
            // Остановка. Как-то просмотрели торможение.
            _ptr_acf->vcl_condition.speed = 0;
            _ptr_acf->vcl_condition.acceleration = 0;
            _ptr_acf->vcl_condition.target_speed = 0;
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
