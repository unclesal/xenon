// *********************************************************************************************************************
// *                                          Самолет выполняет действие полета                                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 14:24 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

#define PREVIOUS_ARRAY_SIZE 50

namespace xenon {

    class AircraftDoesFlying : public AircraftAbstractAction {        
    
        public:
            
            AircraftDoesFlying(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesFlying() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override;
            void _internal_start() override;
            
        private:
            
            float __previous_delta[ PREVIOUS_ARRAY_SIZE ];
            
            void __head_bearing( const float & elapsed_since_last_call );
        
    }; // AircraftDoesFlying
    
}; // namespace xenon
