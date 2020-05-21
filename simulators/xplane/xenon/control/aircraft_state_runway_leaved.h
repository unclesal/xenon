// *********************************************************************************************************************
// *                               Самолет освободил ВПП и дальше пойдет руление на стоянку                            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 14:19 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateRunwayLeaved : public AircraftAbstractState { 
    
        public:
            
            AircraftStateRunwayLeaved(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd 
            );
            
            ~AircraftStateRunwayLeaved() override = default;
            
    }; // class AircraftStateRunwayLeaved
    
}; // namespace xenon
