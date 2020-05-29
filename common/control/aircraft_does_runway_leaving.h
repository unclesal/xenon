// *********************************************************************************************************************
// *                  Самолет завершил посадку и освобождает ВПП для дальнейшей рулежки на стоянку                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 21 may 2020 at 16:21 *
// *********************************************************************************************************************
#pragma once

#include "aircraft_abstract_action.h"

namespace xenon {
    
    class AircraftDoesRunwayLeaving : public AircraftAbstractAction {
        
        public:
        
            AircraftDoesRunwayLeaving(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d
            );
            ~AircraftDoesRunwayLeaving() override = default;
            
        protected:
            
            void _internal_start() override;
            void _internal_step( const float & elapsed_since_last_call ) override;
            
        private:
        
    }; // class AircraftDoesRunwayLeaving
    
}; // namespace xenon
