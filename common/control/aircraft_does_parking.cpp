// *********************************************************************************************************************
// *          Парковка. По сути, то же самое "выравнивание", но с другими скоростями и последовательностями фаз.       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 04 jun 2020 at 09:43 *
// *********************************************************************************************************************
#include "aircraft_does_parking.h"
using namespace xenon;
using namespace std;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesParking::AircraftDoesParking(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction ( ptr_acf, edge_d )
{
    __phase = PHASE_UNKNOWN;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                  Старт действия                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::_internal_start() {
    
    auto wp = _ptr_acf->flight_plan.get(0);
    if ( wp.type != WAYPOINT_PARKING ) _ptr_acf->flight_plan.pop_front();
    
    __phase = PHASE_BECOMING;
    
    // Скорости потихоньку до минимума. Из расчета, что за сколько-то
    // секунд мы этот самый искомый минимум - достигнем.

    
    _taxi_breaking( PARKING_SPEED, 3.0 );
    
    // Курс фиксируем как он был на прошлой фазе.
    _ptr_acf->vcl_condition.heading_acceleration = 0.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Один шаг в режиме подхода к парковочной прямой                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::__becoming( const waypoint_t & wp, const float & elapsed_since_last_call ) {
    
    _head_steering( elapsed_since_last_call, 3.0);
    
    if ( _taxi_turn_started( wp ) ) {
        __phase = PHASE_TURN;
    }
    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Один шаг в фазе поворота                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::__turn( const waypoint_t & wp, const float & elapsed_since_last_call ) {   

    double delta = _get_delta_to_target_heading( wp );
    normalize_degrees( delta );
    
//     Logger::log(
//         "Turn: " + waypoint_to_string( wp.type ) 
//         + ", h=" + to_string( _ptr_acf->get_rotation().heading ) 
//         + ", b=" + to_string( xenon::bearing( _ptr_acf->get_location(), wp.location ) )
//         + ", d=" + to_string( delta )
//     );
    
    if (( abs( delta ) <= 5.0 ) || ( abs(delta) >= 355.0)) {
        __phase = PHASE_STRAIGHT;
        _ptr_acf->vcl_condition.heading_acceleration = 0.0;
        _ptr_acf->vcl_condition.target_heading = _ptr_acf->get_rotation().heading;
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Один шаг в фазе прямолинейного захода на стоянку                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::__straight( const waypoint_t & wp, const float & elapsed_since_last_call ) {
    
    _head_steering( elapsed_since_last_call, 15.0);
    
    auto dis = xenon::distance2d( _ptr_acf->get_location(), wp.location );
    
    auto speed = _ptr_acf->vcl_condition.speed;
    
    if ( speed == 0.0 ) {
        // На ноль делить нельзя, поэтому фазы торможения и не будет вообще.
        _ptr_acf->vcl_condition.acceleration = 0.0;
        _ptr_acf->vcl_condition.speed = 0.0;
        _ptr_acf->vcl_condition.target_speed = 0.0;
        _ptr_acf->flight_plan.pop_front();
        Logger::log(_ptr_acf->vcl_condition.agent_name + " parked.");
        _finish();
        return;
    };

    auto t = dis / speed;
        
    if ( t <= 5.0 ) {
        Logger::log("set breaking, dis=" + to_string( dis ) + ", speed=" + to_string( _ptr_acf->vcl_condition.speed ) + ", time=" + to_string(t));
        __phase = PHASE_BREAKING;
        _ptr_acf->vcl_condition.heading_acceleration = 0.0;
        _ptr_acf->vcl_condition.target_heading = _ptr_acf->get_rotation().heading;        
        _taxi_breaking( 0.0, 5.0 );
    };    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Один шаг в фазе торможения на стоянке                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::__breaking( const float & elapsed_since_last_call ) {

    if ( _ptr_acf->vcl_condition.speed <= 0.2 ) {

        _ptr_acf->vcl_condition.speed = 0.0;
        _ptr_acf->vcl_condition.acceleration = 0.0;
        _ptr_acf->vcl_condition.target_speed = 0.0;
        _ptr_acf->flight_plan.pop_front();
        Logger::log(_ptr_acf->vcl_condition.agent_name + " parked.");
        _finish();

    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Шаг действия                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::_internal_step( const float & elapsed_since_last_call ) {

    auto wp = _ptr_acf->flight_plan.get(0);

    switch ( __phase ) {
        case PHASE_BECOMING: __becoming( wp, elapsed_since_last_call ); break;
        case PHASE_TURN: __turn( wp, elapsed_since_last_call ); break;
        case PHASE_STRAIGHT: __straight( wp, elapsed_since_last_call ); break;
        case PHASE_BREAKING: __breaking( elapsed_since_last_call ); break;
        default: Logger::log("AircraftDoesParking::step(), unhandled phase" + to_string( __phase ));
    };
}
