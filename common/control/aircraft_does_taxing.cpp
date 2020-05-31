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
    
    __choose_speed();    
        
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *               Выбираем скорость в зависимости от расстояния до нулевой точки полетного плана                     *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftDoesTaxing::__choose_speed() {
    auto front_wp = _get_front_wp();
    
    if ( front_wp.type == WAYPOINT_RUNWAY ) {
        if ( _ptr_acf->vcl_condition.target_speed != TAXI_SLOW_SPEED ) {
            
            // _params.tug = TAXI_SLOW_TUG;
            // _params.target_acceleration = TAXI_SLOW_ACCELERATION;
            
            _ptr_acf->vcl_condition.acceleration = 0.0;
            
        }
        return;
    };
    
    auto distance_to_turn = _calculate_distance_to_turn();    
    if (( distance_to_turn > 150.0 ) && ( _ptr_acf->vcl_condition.target_speed != TAXI_NORMAL_SPEED )) {
        
        Logger::log("set TAXI_NORMAL_SPEED, distance=" + to_string( distance_to_turn ) + ", target=" + to_string(_ptr_acf->vcl_condition.target_speed));
        // _params.tug = TAXI_NORMAL_TUG;
        // _params.target_acceleration = TAXI_NORMAL_ACCELERATION;        
        
        _ptr_acf->vcl_condition.acceleration = 0.0;        
        _ptr_acf->vcl_condition.target_speed = TAXI_NORMAL_SPEED;
        
    } else if ( distance_to_turn <= 150.0 ) {
        
        if ( ( _ptr_acf->vcl_condition.speed > TAXI_SLOW_SPEED ) && ( _ptr_acf->vcl_condition.target_speed != TAXI_SLOW_SPEED ) ) {
            
            // Скорость - высокая. Тормозим. 
            // _params.tug = -0.25;
            // _params.target_acceleration = -5.0;
            
            _ptr_acf->vcl_condition.acceleration = 0.0; // target_accel;
            _ptr_acf->vcl_condition.target_speed = TAXI_SLOW_SPEED;
            
        } else if ( _ptr_acf->vcl_condition.speed < TAXI_SLOW_SPEED ) {
            
            Logger::log("Speed up to taxi slow speed.");
            // Текущая скорость низкая, можно подразогнаться до TAXI_SLOW_SPEED
            // _params.tug = TAXI_SLOW_TUG;
            // _params.target_acceleration = TAXI_SLOW_ACCELERATION;
            
            _ptr_acf->vcl_condition.acceleration = 0.0;            
            _ptr_acf->vcl_condition.target_speed = TAXI_SLOW_SPEED;
        }
    }
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                               Перекрытая функция "шага"                                          *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftDoesTaxing::_internal_step( const float & elapsed_since_last_call ) {
    
    // Если в FP нет ВПП, то будет ровно 0. И здесь намеренно берется
    // не первая точка из полетного плана, т.к. между RWY и нами 
    // могут быть еще точки руления, которые будут проигнорированы.
    auto distance_to_rwy = _calculate_distance_to_runway();
    if (( distance_to_rwy > 0.0 ) && ( distance_to_rwy <= 100.0 ) ) {
        
        // подходим к HP.
        
        // Проверка на "целевую скорость" - чтобы войти в режим торможения только один раз.
        // Дальше будет меняться ускорение и его обнулять здесь будет уже нельзя.

        if ( _ptr_acf->vcl_condition.target_speed != 0.0 ) {
            Logger::log("Stopping before HP");
            // _params.tug = -0.1;
            // _params.target_acceleration = -1.0;
            
            _ptr_acf->vcl_condition.acceleration = 0.0;
            _ptr_acf->vcl_condition.target_speed = 0.0;
            _ptr_acf->vcl_condition.heading_acceleration = 0.0;
        }
        
        if ( abs(_ptr_acf->vcl_condition.speed) <= 0.2 ) {
            Logger::log("Full stop on HP. Distance=" + to_string(distance_to_rwy));
            Logger::log(
                "Lat=" + to_string( _get_acf_location().latitude ) 
                + ", lon=" + to_string( _get_acf_location().longitude ) 
                + ", heading=" + to_string( _get_acf_rotation().heading )
            );
            // _params.tug = 0.0;
            // _params.target_acceleration = 0.0;
            
            _ptr_acf->vcl_condition.speed = 0.0;            
            _ptr_acf->vcl_condition.acceleration = 0.0;            
            _ptr_acf->vcl_condition.heading_acceleration = 0.0;
            // Действие было полностью выполнено, выходим.
            auto wp = _get_front_wp();
            while ( wp.type == WAYPOINT_TAXING ) {
                _front_wp_reached();
                wp = _get_front_wp();
            }
            _finish();
            return;            
        }

    } else __choose_speed();
    
    auto wp = _get_front_wp();        
    auto bearing = xenon::bearing( _get_acf_location(), wp.location );
    auto heading = _get_acf_rotation().heading;
    auto delta = bearing - heading;        
    _ptr_acf->vcl_condition.target_heading = bearing;
    _ptr_acf->vcl_condition.heading_acceleration = 25.0 * delta * elapsed_since_last_call;
    
    double distance = _calculate_distance_to_wp( wp );    

    if ( ( distance < 30.0 ) && ( wp.type == WAYPOINT_TAXING ) ) { // || ( _front_wp_recedes() ) ) {
        _front_wp_reached();
        return;
    }
    
}