// *********************************************************************************************************************
// *                    "Медленное перемещение" - при заруливании на стоянку, либо при выруливании из нее              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:18 *
// *********************************************************************************************************************

#include "aircraft_does_slow_taxing.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftDoesSlowTaxing::AircraftDoesSlowTaxing ( 
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d  
) : AircraftAbstractAction( ptr_acf, edge_d )
{
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                        Процедура старта данного действия                                          *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesSlowTaxing::_internal_start() {
    _ptr_acf->is_clamped_to_ground = true;
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                             Один шаг данного действия                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AircraftDoesSlowTaxing::_internal_step ( const float & elapsed_since_last_call ) {
}

