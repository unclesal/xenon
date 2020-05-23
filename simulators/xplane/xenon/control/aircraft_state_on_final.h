// *********************************************************************************************************************
// *                                               Самолет вышел на глиссаду                                           *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 14:09 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateOnFinal : public AircraftAbstractState {
        
        public:
            
            AircraftStateOnFinal( AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd );
            ~AircraftStateOnFinal() override = default;
            
        protected:
            
            void _internal_activate() override {};

    };
    
}; // namespace xenon

