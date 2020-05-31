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
            void _internal_step( const float & elapsed_since_last_call ) override;
            
            
        private:
            
            enum phase_t {
                PHASE_UNKNOWN = 0,
                PHASE_DESCENDING,
                // Выравнивание и выдерживание - в
                // одной фазе, вплоть до касания ВПП.
                PHASE_ALIGNMENT,
                // Торможение
                PHASE_BREAKING,
                // Уход с ВПП на ближайшую рулежку
                PHASE_LEAVE_RUNWAY
            };
            
            phase_t __phase;
            
            bool __flaps_to_take_off_position;
            bool __flaps_to_landing_position;
                        
            void __step__descending( const waypoint_t & wp, const aircraft_parameters_t & acf_parameters );
            void __step__alignment( const waypoint_t & wp, const aircraft_parameters_t & acf_parameters, const float & elapsed_since_last_call );
            void __step__breaking();
            
    }; //AircraftDoesLanding
    
}; // namespace xenon