// *********************************************************************************************************************
// *                          Выталкивание самолета (действие, ребро графа состояний самолета)                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 15 may 2020 at 14:22 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesPushBack : public AircraftAbstractAction {
        
        public:
            
            enum phase_t {
                PHASE_NONE = 0,
                PHASE_STRAIGHT,
                PHASE_TURN,
                PHASE_STOP
            };
            
            AircraftDoesPushBack(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d
            );
            ~AircraftDoesPushBack() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override;
            void _internal_start() override;
    
        private:
            
            phase_t __current_phase;
            
            double __get_delta_to_target_heading( const waypoint_t & wp );
            void __internal_step__phase_straight();
            void __internal_step__phase_turn();
            void __internal_step__phase_stop();
        
    }; // class AircraftDoesPushBack
    
}; // namespace xenon
