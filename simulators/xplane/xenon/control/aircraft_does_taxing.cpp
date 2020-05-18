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
    
    _acf_will_on_ground( true );
    _ptr_acf->set_taxi_lites( true );
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
    auto distance = _calculate_distance_to_wp( front_wp );
    if (( distance > 100.0 ) && ( _params.target_speed != TAXI_NORMAL_SPEED )) {
        
        XPlane::log("set TAXI_NORMAL_SPEED, distance=" + to_string( distance ) + ", target=" + to_string(_params.target_speed));
        _params.tug = TAXI_NORMAL_TUG;
        _params.target_acceleration = TAXI_NORMAL_ACCELERATION;
        _params.target_speed = TAXI_NORMAL_SPEED;
        
    } else if ( distance <= 100.0 ) {
        
        // Если расстояние меньше скольки-то - то тут одно
        // из двух. Либо едем медленно, либо и вовсе тормозим.
        // Надо тормозить или нет, определяется разницей в курсах.
        auto delta = front_wp.incomming_heading - front_wp.outgoing_heading;
        normalize_degrees( delta );
        double dt = 45.0;
        bool need_breaking = ( ( delta < 360.0-dt ) && ( delta > dt ) );
        if (( _params.speed > TAXI_SLOW_SPEED ) && ( need_breaking )) {
            XPlane::log("Breaking...");
            // Скорость - высокая. Тормозим. Причем с таким расчетом, чтобы
            // к указанной точке выйти на TAXI_SLOW_SPEED
            _params.tug = 0.0;
            _params.target_speed = TAXI_SLOW_SPEED;
            _params.target_acceleration = ( _params.target_speed - _params.speed ) / distance;
            
        } else if ( _params.speed < TAXI_SLOW_SPEED ) {
            XPlane::log("Speed up to taxi slow speed.");
            // Текущая скорость низкая, можно подразогнаться до TAXI_SLOW_SPEED
            _params.tug = TAXI_SLOW_TUG;
            _params.target_acceleration = TAXI_SLOW_ACCELERATION;
            _params.target_speed = TAXI_SLOW_SPEED;
        }
    }
}

// ********************************************************************************************************************
// *                                                                                                                  *
// *                                               Перекрытая функция "шага"                                          *
// *                                                                                                                  *
// ********************************************************************************************************************

void AircraftDoesTaxing::_internal_step( const float & elapsed_since_last_time ) {
    
    __choose_speed();
    
    auto wp = _get_front_wp();
    double distance = _calculate_distance_to_wp( wp );
    
    if ( _params.speed > 1.0 ) {

        // Если есть хоть какая-то скорость, то можно попытаться подстраивать курс.
        auto bearing = xenon::bearing( _get_acf_location(), wp.location );
        auto heading = _get_acf_rotation().heading;
        auto delta = bearing - heading;
        
        // XPlane::log("Bearing=" + to_string( bearing ) + ", heading =" + to_string( heading ) + ", delta=" + to_string( delta ) );
                
        _params.target_heading = bearing;
        double heading_acceleration_delta = delta / _params.speed;
        delta < 0 ? _params.heading_acceleration = - heading_acceleration_delta : _params.heading_acceleration = heading_acceleration_delta;
    };
    
    // XPlane::log("distance=" + to_string(distance) + ", recedes=" + to_string( _front_wp_recedes()));
    
    if ( distance < TAXI_TURN_RADIUS ) { // || ( _front_wp_recedes() ) ) {
        XPlane::log("waypoint reached.");
        _front_wp_reached();
    }    
    
}
