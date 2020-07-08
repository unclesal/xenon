// *********************************************************************************************************************
// *                                                   Агент самолета                                                  *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 08:31 *
// *********************************************************************************************************************
#pragma once

#include <unistd.h>

#include <string>
#include <map>
#include <deque>
#include <mutex>

#include "airport.h"
#include "abstract_agent.h"
#include "agent_interface.h"
#include "bimbo_aircraft.h"
#include "aircraft_abstract_state.h"
#include "state_frame.h"

namespace xenon {
    
    class AgentAircraft : public AbstractAgent, public AgentInterface {
        
        public:
            
            AgentAircraft( 
                const std::string & uuid
            );

            virtual ~AgentAircraft();
            
            void run();
            
            void on_connect() override;
            void on_disconnect() override;            
            
            void on_error( std::string message ) override;
            
            virtual void action_started( void * action ) override;            
            virtual void action_finished( void * action ) override;            
            virtual void state_changed( void * state ) override;
            
            /**
             * @short Прореветь об изменении своего состояния
             * Посылка пакета CmdAircraftState, если коммуникатор соединен.
             */
            void scream_about_me();
            virtual void on_received( void * abstract_command ) override;
            void on_received( CmdAircraftCondition * cmd );
            
        protected:                        
            
        private:

            long int __previous_time;
            unsigned int __cycles;

            BimboAircraft * __ptr_acf;
            map<aircraft_state_t, vector<StateFrame *>> __state_frames;
            
            long int __start_time;
            bool __started;
            
            void __init_parking_frames();
            void __init_taxing_frames();
            void __step();

            // void __test();
            void __temporary_make_aircraft_by_uuid( const std::string & uuid );
            
            //void __choose_next_action();
            
            void __decision();
            void __start_fp0_action();
            
            
            
            
    };
    
}; // namespace xenon
