// *********************************************************************************************************************
// *                  "Становление" самолета, переход из состояния "взлетел" в состояние "в полете"                    *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 11 jul 2020 at 10:55 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesBecoming : public AircraftAbstractAction {
    
        public:
            AircraftDoesBecoming(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesBecoming() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override;
            void _internal_start() override;
            
        private:
        
    };
    
}; // namespace xenon
