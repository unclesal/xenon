// *********************************************************************************************************************
// *                      Интерфейс реакций на события в (якобы уже присоединенном) коммуникатореа                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 26 may 2020 at 15:13 *
// *********************************************************************************************************************
#pragma once

#include <string>
#include <vector>
#include <mutex>

namespace xenon {

    class ConnectedCommunicatorReactor {
        
        public:

            struct another_agent_t {
                vehicle_condition_t vcl_condition;
                aircraft_condition_t * acf_condition = nullptr;
                
                const std::string & agent_uuid() {
                    return vcl_condition.agent_uuid;
                };
            };
            
            ConnectedCommunicatorReactor() = default;
            virtual ~ConnectedCommunicatorReactor() = default;
            
            std::vector<another_agent_t> agents;
            std::mutex agents_mutex;
                    
            virtual void on_connect() = 0;
            virtual void on_disconnect() = 0;
            virtual void on_received( void * abstract_command ) = 0;
            virtual void on_error( std::string message ) = 0;
            
        protected:                        
            

    }; // class ConnectedCommunicatorReactor

}; // namespace xenon
