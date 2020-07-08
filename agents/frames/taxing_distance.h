// *********************************************************************************************************************
// *                          Фрейм, отслеживающий соблюдение дистанции между самолетами при рулении                   *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 08 jul 2020 at 17:00 *
// *********************************************************************************************************************
#pragma once
#include "state_frame.h"

namespace xenon {
    class TaxingDistance : public StateFrame {
        
        public:
            
            TaxingDistance( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment );
            ~TaxingDistance() override = default;
            
            void update( CmdAircraftCondition * cmd ) override;
            
        protected:
            
        private:
            
    }; // class TaxingDistance
}; // namespace xenon
