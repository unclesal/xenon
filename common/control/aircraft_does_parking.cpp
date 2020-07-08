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
    
    __phase = PHASE_BECOMING;
    
    // Скорости потихоньку до минимума. Из расчета, что за сколько-то
    // секунд мы этот самый искомый минимум - достигнем.

    _ptr_acf->vcl_condition.target_speed = PARKING_SPEED;
    float ds = PARKING_SPEED - _ptr_acf->vcl_condition.speed;
    _ptr_acf->vcl_condition.acceleration = ds / 5.0;    
    
    // Курс фиксируем как он был на прошлой фазе.
    _ptr_acf->vcl_condition.heading_acceleration = 0.0;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Один шаг в режиме подхода к парковочной прямой                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::__becoming( const waypoint_t & wp, const float & elapsed_since_last_call ) {
    
    auto loc1 = xenon::shift( wp.location, 25, wp.incomming_heading );
    auto dis = xenon::distance_to_segment( _get_acf_location(), wp.location, loc1 );
    auto delta = _get_delta_bearing( wp );
    
    if ( dis <= 30.0 ) {
        _ptr_acf->vcl_condition.target_heading = wp.incomming_heading;
        _ptr_acf->vcl_condition.heading_acceleration = delta / 25.0;
        __phase = PHASE_TURN;
    }    
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Один шаг в фазе поворота                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::__turn( const waypoint_t & wp, const float & elapsed_since_last_call ) {    
    auto delta = _get_delta_bearing( wp );
    if (( abs( delta ) <= 1.0 ) || ( abs(delta) >= 359.0)) {
        __phase = PHASE_STRAIGHT;
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                  Один шаг в фазе прямолинейного захода на стоянку                                 *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::__straight( const waypoint_t & wp, const float & elapsed_since_last_call ) {
    _head_steering( elapsed_since_last_call, 10.0);
    auto dis = _calculate_distance_to_wp( wp );
    
    auto speed = _ptr_acf->vcl_condition.speed;
    if ( speed == 0.0 ) {
        // На ноль делить нельзя, поэтому фазы торможения и не будет вообще.
        _ptr_acf->vcl_condition.acceleration = 0.0;
        _ptr_acf->vcl_condition.speed = 0.0;
        _front_wp_reached();
        _finish();
        return;
    };
    auto t = dis / speed;
    // XPlane::log("straight, dis=" + to_string( dis ) + ", speed=" + to_string( _ptr_acf->vcl_condition.speed ) + ", time=" + to_string(t));
    if ( t <= 5.0 ) {
        __phase = PHASE_BREAKING;
        _taxi_breaking( 0.0, 5.0 );
    };
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                       Один шаг в фазе торможения на стоянке                                       *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::__breaking() {
    if ( _ptr_acf->vcl_condition.speed <= 0.1 ) {
        _ptr_acf->vcl_condition.speed = 0.0;
        _ptr_acf->vcl_condition.acceleration = 0.0;
        _front_wp_reached();
        _finish();
    }
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Шаг действия                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesParking::_internal_step( const float & elapsed_since_last_call ) {
    auto wp = _ptr_acf->front_waypoint();
    switch ( __phase ) {
        case PHASE_BECOMING: __becoming( wp, elapsed_since_last_call ); break;
        case PHASE_TURN: __turn( wp, elapsed_since_last_call ); break;
        case PHASE_STRAIGHT: __straight( wp, elapsed_since_last_call ); break;
        case PHASE_BREAKING: __breaking(); break;
        default: Logger::log("AircraftDoesParking::step(), unhandled phase" + to_string( __phase ));
    };
}
