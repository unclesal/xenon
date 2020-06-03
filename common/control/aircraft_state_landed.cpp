// *********************************************************************************************************************
// *                                    Приземлился ( закончил фазу торможения на ВПП )                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 03 jun 2020 at 10:58 *
// *********************************************************************************************************************

#include "aircraft_state_landed.h"

using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AircraftStateLanded::AircraftStateLanded( 
    AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd )
    : AircraftAbstractState ( ptr_acf, nd ) 
{
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                           Внутренняя активация состояния                                          *
// *                                                                                                                   *
// *********************************************************************************************************************
void AircraftStateLanded::_internal_activate() {
    
    auto acf_parameters = _get_acf_parameters();
    auto our_location = _get_acf_location();
    auto our_heading = _get_acf_rotation().heading;
    
    auto airport = Airport::get_by_icao( acf_parameters.destination );
    auto parking = airport.get_free_parking( _ptr_acf->acf_condition.icao_type );
    auto way = airport.get_taxi_way_for_parking( our_location, our_heading, parking );
    _ptr_acf->prepare_for_taxing( way );

}
