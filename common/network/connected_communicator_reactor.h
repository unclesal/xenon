// *********************************************************************************************************************
// *                      Интерфейс реакций на события в (якобы уже присоединенном) коммуникатореа                     *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 26 may 2020 at 15:13 *
// *********************************************************************************************************************
#pragma once

#include <string>
#include <vector>
#include <mutex>

#include "flight_plan.h"
#include "utils.hpp"

namespace xenon {

    class ConnectedCommunicatorReactor {
        
        public:

            struct another_agent_t {
                
                vehicle_condition_t vcl_condition;
                
                aircraft_condition_t * acf_condition = nullptr;
                
                FlightPlan * flight_plan = nullptr;
                
                const std::string & agent_uuid() {
                    return vcl_condition.agent_uuid;
                };
                
                bool is_aircraft() {
                    return (
                        vcl_condition.agent_type == AGENT_AIRCRAFT
                        || vcl_condition.agent_type == AGENT_XPLANE
                    );
                };
                                                
                /**
                 * @short Находится ли данный агент в состоянии руления?
                 */
                bool is_on_taxiway () {
                                        
                    return (
                        vcl_condition.current_action == ACF_DOES_PUSH_BACK
                        || vcl_condition.current_action == ACF_DOES_SLOW_TAXING
                        || vcl_condition.current_action == ACF_DOES_NORMAL_TAXING
                        || vcl_condition.current_action == ACF_DOES_TAXING_STOP
                        || vcl_condition.current_action == ACF_DOES_WAITING_TAKE_OFF_APPROVAL
                        || vcl_condition.current_action == ACF_DOES_LINING_UP
                        || vcl_condition.current_action == ACF_DOES_TAKE_OFF
                        || vcl_condition.current_state == ACF_STATE_MOTION_STARTED
                        || vcl_condition.current_state == ACF_STATE_READY_FOR_TAXING
                    );
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
