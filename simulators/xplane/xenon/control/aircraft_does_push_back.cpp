// *********************************************************************************************************************
// *                          Выталкивание самолета (действие, ребро графа состояний самолета)                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:27 *
// *********************************************************************************************************************

#include "aircraft_does_push_back.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesPushBack::AircraftDoesPushBack(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction( ptr_acf, edge_d )
{
    __current_phase = PHASE_STRAIGHT;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                          Внутренняя процедура начала действия                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::_internal_start() {
    
    _ptr_acf->set_nav_lites( true );
    _tug = - 0.005;
    _target_acceleration = -TAXI_ACCELERATION;
    _target_speed = PUSH_BACK_SPEED;

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Один шаг в выполнении данного действия                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesPushBack::_internal_step ( float elapsed_since_last_time ) {
    
    if ( __current_phase == PHASE_STRAIGHT ) {
        // Точка, до которой нас выталкивают.
        waypoint_t wp = _get_front_wp();
        auto wp_position = XPlane::location_to_position( wp.location );
        
        // Описатель "целевой рулежной дорожки", куда мы собрались приехать.
        // TODO: переделать на глобальные координаты
        line_descriptor_t final_taxiway = XPlane::line( wp_position, wp.rotation.heading );

        // "Плоское" расстояние от места положения самолета до этой самой прямой линии.
        double distance = XPlane::distance_2d( wp_position, final_taxiway );
        XPlane::log("Distance=" + to_string( distance ) );
        if ( distance <= TAXI_TURN_RADIUS ) {
            __current_phase = PHASE_TURN;
            // Определение направления повотора, вправо или влево.
        }
    }
    
}



