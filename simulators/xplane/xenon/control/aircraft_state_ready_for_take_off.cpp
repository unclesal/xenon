// *********************************************************************************************************************
// *                                                  Состояние "готов к взлету"                                       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 19 may 2020 at 20:24 *
// *********************************************************************************************************************

#include "aircraft_state_ready_for_take_off.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                        Конструктор                                                *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftStateReadyForTakeOff::AircraftStateReadyForTakeOff(
    xenon::AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd
) : AircraftAbstractState( ptr_acf, nd )
{
}
