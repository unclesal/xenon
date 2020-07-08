// *********************************************************************************************************************
// *                                   Торможение при рулении, вплоть до полной остановки                              *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 08 jul 2020 at 16:41 *
// *********************************************************************************************************************
#pragma once
#include "aircraft_abstract_action.h"

namespace xenon {
    class AircraftDoesTaxingStop : public AircraftAbstractAction {
        
        public:
            
            AircraftDoesTaxingStop(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::edge_descriptor & edge_d 
            );
            ~AircraftDoesTaxingStop() override = default;
            
        protected:
            
            void _internal_step( const float & elapsed_since_last_call ) override ;
            void _internal_start() override;
            
        private:
            
    }; // class AircraftDoesTaxingStop
    
}; // namespace xenon
