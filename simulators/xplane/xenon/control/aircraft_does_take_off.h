// *********************************************************************************************************************
// *                                                    Действие взлета                                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 20 may 2020 at 12:10 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesTakeOff : public AircraftAbstractAction {
        
        public:
            
            enum phase_t {
                PHASE_NOTHING = 0,
                // Разбег
                PHASE_RUN_UP,
                // Отрыв
                PHASE_BREAK_AWAY,
                // Набор высоты.
                PHASE_CLIMBING
            };
            
            AircraftDoesTakeOff(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesTakeOff() override = default;
        
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override ;
            void _internal_start() override;
        
        private:
            
            phase_t __phase;
            float __gear_up_altitude;
            
            void __step__run_up( const float & elapsed_since_last_call );
            void __step__break_away ( const float & elapsed_since_last_call );
            void __step__climbing( const float & elapsed_since_last_call );
        
    };
    
};

