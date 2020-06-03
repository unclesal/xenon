// *********************************************************************************************************************
// *                                Руление закончено и осталась одна точка - это сама стоянка.                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 03 jun 2020 at 11:18 *
// *********************************************************************************************************************

#include "aircraft_state_before_parking.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftStateBeforeParking::AircraftStateBeforeParking(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd
) : AircraftAbstractState ( ptr_acf, nd ) 
{
}