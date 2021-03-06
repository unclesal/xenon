// *********************************************************************************************************************
// *                                                   Ничегонеделание                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 18:26 *
// *********************************************************************************************************************
#include "aircraft_does_nothing.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор.                                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesNothing::AircraftDoesNothing(
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
) : AircraftAbstractAction( ptr_acf, edge_d ) 
{
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Перекрытая функция старта действия                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesNothing::_internal_start() {
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                   Перекрытая функция "внутреннего шага" действия                                  *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesNothing::_internal_step( const float & elapsed_since_last_call ) {
    if ( _total_duration >= 10.0f ) _finish();
};

