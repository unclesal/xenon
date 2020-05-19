// *********************************************************************************************************************
// *                                           Состояние "на предварительном старте"                                   *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 19 may 2020 at 19:48 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateOnHP : public AircraftAbstractState {
        
        public:
            AircraftStateOnHP(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd
            );
            ~AircraftStateOnHP() override = default;
    };
    
}; // namespace xenon
