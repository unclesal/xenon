// *********************************************************************************************************************
// *                                                    Взлет произведен                                               *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 20 may 2020 at 12:03 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateAirborned : public AircraftAbstractState {
        
        public:
            
            AircraftStateAirborned( AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd );
            ~AircraftStateAirborned() override = default;
            
    };
    
};
