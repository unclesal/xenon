// *********************************************************************************************************************
// *         Любой внешний агент, который может общаться с коммуникатором и через него - с другими агентами            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 05:51 *
// *********************************************************************************************************************
#pragma once

#include <unistd.h>

#include <string>
#include <iostream>

#include "structures.h"
#include "connected_communicator.h"
#include "connected_communicator_reactor.h"
#include "airport.h"

namespace xenon {
    
    class AbstractAgent : public ConnectedCommunicatorReactor {
        
        public:                        
            
            AbstractAgent();
            virtual ~AbstractAgent();
            
            virtual void run() = 0;
            virtual void on_received( void * abstract_command ) override;            

        protected:
            
            ConnectedCommunicator * _communicator; 
            
            inline void _create_communicator() {
                _communicator = new ConnectedCommunicator( this );
            };                        
            
        private:            
            
    }; // AbstractAgent
    
}; // xenon
