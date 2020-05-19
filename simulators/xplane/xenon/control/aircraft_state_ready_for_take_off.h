// // *********************************************************************************************************************
// // *                                                  Состояние "готов к взлету"                                       *
// // *                                                                                                                   *
// // * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 19 may 2020 at 20:24 *
// // *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateReadyForTakeOff : public AircraftAbstractState {
    
        public:
            AircraftStateReadyForTakeOff( AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd );
            ~AircraftStateReadyForTakeOff() override = default;
            
    }; // class AircraftStateReadyForTakeOff
    
}; // namespace xenon
