// *********************************************************************************************************************
// *                                          Самолет выполняет действие полета                                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 14:24 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

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
            
            enum phase_t {
                PHASE_UNKNOWN = 0,
                PHASE_WAYPOINT_CONTROLLED,
                PHASE_CLIMBING,
                PHASE_CRUISING,
                PHASE_DESCENDING
                // Фазы "посдки" здесь нет, т.к. 
                // для посадки предусмотрено отдельное действие.
            };
            
            phase_t __phase;
            
            void __control_of_speed ( const waypoint_t & waypoint, const float & time_to_achieve );
            void __control_of_altitude( const waypoint_t & waypoint, const float & time_to_achieve );
                    
    }; // AircraftDoesFlying
    
}; // namespace xenon
