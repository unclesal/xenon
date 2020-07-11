// *********************************************************************************************************************
// *                                 Состояние подхода к ВПП с целью посадки (зашел в растр ILS                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 11 jul 2020 at 08:51 *
// *********************************************************************************************************************

#include "aircraft_state_approach.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                     Конструктор                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftStateApproach::AircraftStateApproach(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd
) : AircraftAbstractState( ptr_acf, nd )
{
}