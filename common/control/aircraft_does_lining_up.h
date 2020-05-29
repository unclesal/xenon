// *********************************************************************************************************************
// *                                              Выравнивание на ВПП для взлета                                       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 19 may 2020 at 20:45 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesLiningUp : public AircraftAbstractAction {
    
        public:
            
            enum phase_t {
                PHASE_NOTHING = 0,
                PHASE_STRAIGHT,
                PHASE_ROTATION
            };
            
            AircraftDoesLiningUp(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesLiningUp() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override;
            void _internal_start() override;
            
        private:
            
            phase_t __phase;
            
            void __step_straight( const float & elapsed_since_last_call );
            void __step_rotation( const float & elapsed_since_last_call );
        
    }; // class AircraftDoesLiningUp
    
};

