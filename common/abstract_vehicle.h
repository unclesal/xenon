// *********************************************************************************************************************
// *                      Любой объект, который существует внутри X-Plane и может перемещаться                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:45 *
// *********************************************************************************************************************
#pragma once

#include "structures.h"
#include "utils.hpp"
#include "logger.h"
#include "connected_communicator.h"

#ifdef INSIDE_XPLANE
#include "XPLMScenery.h"
#endif

#ifdef INSIDE_AGENT
#include "agent_interface.h"
#endif

namespace xenon {

    class AbstractVehicle {

        public:

            AbstractVehicle();
            virtual ~AbstractVehicle();

#ifdef INSIDE_AGENT
            void set_agent(AgentInterface * a ) {                
                agent = a;                
            };
#endif
            
#ifdef INSIDE_XPLANE
            virtual position_t get_position() = 0;
            virtual void set_position( const position_t & position ) = 0;
#endif

            virtual location_t get_location();
            virtual void set_location( const location_t & location );
            virtual rotation_t get_rotation() = 0;
            virtual void set_rotation( const rotation_t & rotation ) {};
            
            const std::string & agent_uuid() {
                return vcl_condition.agent_uuid;
            };
            
            void update_from( const vehicle_condition_t & vc ) {
                
                vcl_condition = vc;
                auto new_location = vc.location;
#ifdef INSIDE_XPLANE
                // Попытка убрать дергание меток, раздражает сильно.
                auto new_position = XPlane::location_to_position( vc.location );
                if ( vc.is_clamped_to_ground ) hit_to_ground( new_position );
                auto old_position = get_position();
                auto distance = XPlane::distance2d(old_position, new_position);
                if ( distance >= 0.5 ) set_position( new_position );
                set_rotation( vc.rotation );
#else               
                set_location( new_location );                
                set_rotation( vc.rotation );
#endif                
                                
            };                        

#ifdef INSIDE_XPLANE
            virtual void observe() {};
            // virtual void control( float elapse_since_last_call ) {};
            
            /**
             * @short Установить координату высоты - на земле.
             * Перенесен в "абстрактную самоходку" из-за того, что вроде бы как 
             * время создания XPLMProbeRef достаточно большое и не хочется
             * его создавать каждый раз в цикле перемещения. 
             */
            virtual void hit_to_ground( position_t & position );
            
            /**
             * @short Это то же самое, что и выше, просто обернуто в локацию.
             */
            virtual void hit_to_ground( location_t & location );
            void clamp_to_ground();
#endif
            /**
             * @short "Состояние самоходки".
             * Является публичным членом, т.к. к нему обращаются все: и сама самоходка, и граф, и его узлы/грани.
             */
            vehicle_condition_t vcl_condition;
            
#ifdef INSIDE_AGENT
           AgentInterface * agent;
#endif

        protected:                    
            
            virtual void _action_finished( void * action ) {
#ifdef INSIDE_AGENT
                if ( agent ) agent->action_finished( action );
#endif                
            };            

        private:
            
#ifdef INSIDE_XPLANE
           XPLMProbeRef __terrain_ref;
#endif
           
    };

}; // namespace xenon
