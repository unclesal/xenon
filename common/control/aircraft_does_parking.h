// *********************************************************************************************************************
// *                                           Действие заруливания  на парковку                                       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 04 jun 2020 at 09:43 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    class AircraftDoesParking : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesParking(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesParking() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override;
            void _internal_start() override;
            
        private:
        
            enum phase_t {
                PHASE_UNKNOWN,
                PHASE_BECOMING,
                PHASE_TURN,
                PHASE_STRAIGHT,
                PHASE_BREAKING
            };
            
            phase_t __phase;
            
            void __becoming( const waypoint_t & wp, const float & elapsed_since_last_call );
            void __turn( const waypoint_t & wp, const float & elapsed_since_last_call );
            void __straight( const waypoint_t & wp, const float & elapsed_since_last_call );
            void __breaking( const float & elapsed_since_last_call );
        
    }; // class AircraftDoesParking
}; // namespace xenon

