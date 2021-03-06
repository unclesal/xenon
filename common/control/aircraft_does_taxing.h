// *********************************************************************************************************************
// *                                                Самолет выполняет руление                                          *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 18 may 2020 at 13:55 *
// *********************************************************************************************************************

#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesTaxing : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesTaxing(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesTaxing() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override ;
            void _internal_start() override;
    
        private:
            
            location_t __from_runway_location;
            
            void __choose_speed();

        
    }; // class AircraftDoesTaxing
    
}; // namespace xenon
