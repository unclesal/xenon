// *********************************************************************************************************************
// *                                                   Ничегонеделание                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 18:23 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesNothing : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesNothing(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesNothing() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override;
            void _internal_start() override;
    };
};
