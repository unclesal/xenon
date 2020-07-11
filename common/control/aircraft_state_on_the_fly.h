// *********************************************************************************************************************
// *                                              Состояние "в полете"                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 11 jul 2020 at 10:49 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateOnTheFly : public AircraftAbstractState {
    
        public:
            
            AircraftStateOnTheFly(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd 
            );
            ~AircraftStateOnTheFly() override = default;
            
        protected:
            
            void _internal_activate() override {};
            
        private:
        
    };
    
}; // namespace xenon
