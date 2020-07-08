// *********************************************************************************************************************
// *                                             Самолет ожидает выталкивания.                                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 08 jul 2020 at 20:42 *
// *********************************************************************************************************************

#include "aircraft_does_waiting_push_back.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                   Конструктор                                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesWaitingPushBack::AircraftDoesWaitingPushBack(
    AbstractAircraft* ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d
) : AircraftAbstractAction( ptr_acf, edge_d )
{
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                              Инициализация действия                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesWaitingPushBack::_internal_start() {
    // Стоим. Действие - простейшее. Просто стоим.
    _ptr_acf->vcl_condition.speed = 0;
    _ptr_acf->vcl_condition.target_speed = 0;
    _ptr_acf->vcl_condition.acceleration = 0;
}



