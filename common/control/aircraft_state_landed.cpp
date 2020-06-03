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
    
    XPlane::log("AircraftStateLanded::__internal_activate()");
    auto acf_parameters = _get_acf_parameters();
    XPlane::log("got parameters.");
    auto our_location = _get_acf_location();
    XPlane::log("got our heading...");
    auto our_heading = _get_acf_rotation().heading;
    
    XPlane::log("got airport");
    if ( acf_parameters.destination.empty() ) {
        Logger::log("AircraftStateLanded::_internal_acitvate(), FP without destination.");
        return;
    }
    auto airport = Airport::get_by_icao( acf_parameters.destination );

    if ( _ptr_acf->acf_condition.icao_type.empty() ) {
        Logger::log("AircraftStateLanded::_internal_activate(), aircraft ICAO type empty.");
        return;
    };
    auto parking = airport.get_free_parking( _ptr_acf->acf_condition.icao_type );
    auto way = airport.get_taxi_way_for_parking( our_location, our_heading, parking );
    _ptr_acf->prepare_for_taxing( way );

}
