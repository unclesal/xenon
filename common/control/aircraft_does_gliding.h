// *********************************************************************************************************************
// *                            Заход в растр ILS, снижение. Фактически то же самое, что и посадка                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 11 jul 2020 at 08:58 *
// *********************************************************************************************************************

#pragma once
#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesGliding : public AircraftAbstractAction {
    
        public:
        
            AircraftDoesGliding(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesGliding() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override;
            void _internal_start() override;
            
        private:
            
            
        
    };
};
