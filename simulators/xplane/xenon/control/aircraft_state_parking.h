// *********************************************************************************************************************
// *                                           Состояние, когда самолет на парковке                                    *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 11:25 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"
#include "aircraft_state_graph_definition.h"

namespace xenon {
    
    class AircraftStateParking: public AircraftAbstractState {
        
        public:
            
            AircraftStateParking( 
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd 
            );
            virtual ~AircraftStateParking() = default;
            
    }; // class AircraftStateParking
    
}; // namespace xenon
