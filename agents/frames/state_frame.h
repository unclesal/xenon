// *********************************************************************************************************************
// *                       Фрейм, имеющий отношение к определенному состоянию (узел графа) самолета                    *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 07 jul 2020 at 11:51 *
// *********************************************************************************************************************
#pragma once
#include "abstract_frame.h"
#include "aircraft_abstract_state.h"
#include "connected_communicator_reactor.h"
#include "bimbo_aircraft.h"
#include "cmd_aircraft_condition.h"

namespace xenon {
    
    class StateFrame : public AbstractFrame {
        
        public:
            
            StateFrame( BimboAircraft * bimbo, ConnectedCommunicatorReactor * environment );
            virtual ~StateFrame() override = default;
            
            
            virtual void update( CmdAircraftCondition * cmd ) = 0;
            virtual void result( next_action_t & next_action );
            
        protected:
            
            aircraft_action_t _action;
            int _likeliness;
            
            ConnectedCommunicatorReactor * _environment;
            BimboAircraft * _ptr_acf;
            
        private:
            
            
            
    };
    
}; // namespace xenon
