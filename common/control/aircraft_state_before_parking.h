// *********************************************************************************************************************
// *                                Руление закончено и осталась одна точка - это сама стоянка.                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 03 jun 2020 at 11:10 *
// *********************************************************************************************************************

#pragma once

#include "aircraft_abstract_state.h"

namespace xenon {
    
    class AircraftStateBeforeParking : public AircraftAbstractState {
        
        public:
            
            AircraftStateBeforeParking(
                AbstractAircraft * ptr_acf, const aircraft_state_graph::graph_t::vertex_descriptor & nd
            );
            
            ~AircraftStateBeforeParking() override = default;
            
        protected:

            void _internal_activate() override {};
            
        private:
            
    }; // class AircraftStateBeforeParking
    
}; // namespace xenon
