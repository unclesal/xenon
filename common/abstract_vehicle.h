// *********************************************************************************************************************
// *                      Любой объект, который существует внутри X-Plane и может перемещаться                         *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 02 may 2020 at 10:45 *
// *********************************************************************************************************************
#pragma once

#include "structures.h"
#include "utils.hpp"
#include "logger.h"

#ifdef INSIDE_XPLANE
#include "XPLMScenery.h"
#endif

namespace xenon {

    class AbstractVehicle {

        public:

            AbstractVehicle();
            virtual ~AbstractVehicle();

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
                set_location( vc.location );
                set_rotation( vc.rotation );
#ifdef INSIDE_XPLANE
                if ( vcl_condition.is_clamped_to_ground ) clamp_to_ground();
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

        protected:                        

        private:
            
#ifdef INSIDE_XPLANE
           XPLMProbeRef __terrain_ref;
#endif

    };

}; // namespace xenon
