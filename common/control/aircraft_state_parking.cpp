// *********************************************************************************************************************
// *                                           Состояние, когда самолет на парковке                                    *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 11:27 *
// *********************************************************************************************************************

#include "aircraft_state_parking.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftStateParking::AircraftStateParking(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd  
) : AircraftAbstractState( ptr_acf, nd )
{
}

