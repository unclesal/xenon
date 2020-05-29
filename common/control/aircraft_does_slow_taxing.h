// *********************************************************************************************************************
// *                    "Медленное перемещение" - при заруливании на стоянку, либо при выруливании из нее              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:10 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesSlowTaxing : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesSlowTaxing( 
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesSlowTaxing() override = default;
            
        protected:
            
            virtual void _internal_step( const float & elapsed_since_last_call );
            virtual void _internal_start();

    };
    
}; // namespace xenon
