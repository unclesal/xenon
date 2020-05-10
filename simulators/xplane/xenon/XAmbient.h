// *********************************************************************************************************************
// *                                Представление окружающей действительности - внутри X-Plane.                        *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 14 sep 2019 at 19:40 *
// *********************************************************************************************************************

#pragma once

#include "XPLMDataAccess.h"

#include "Ambient.h"
#include "xplane_utilities.h"

namespace xenon {
    
    class XAmbient : public Ambient {
        
        public:
        
            XAmbient();
            virtual ~XAmbient();
            
            void observe();
            
        protected:
            
        private:
            
            XPLMDataRef __ambient_temperature_dataref;
            XPLMDataRef __y_agl_dataref;
        
    };
    
};
