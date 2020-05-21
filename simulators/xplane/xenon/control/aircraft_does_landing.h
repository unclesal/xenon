// *********************************************************************************************************************
// *                                              Самолет выполняет посадку                                            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 14:36 *
// *********************************************************************************************************************

#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesLanding : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesLanding(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d
            );
            ~AircraftDoesLanding() override = default;
            
        protected:
            
            void _internal_start() override;
            void _internal_step( const float & elapsed_since_last_time ) override;
            
            
        private:
            
    }; //AircraftDoesLanding
    
}; // namespace xenon
