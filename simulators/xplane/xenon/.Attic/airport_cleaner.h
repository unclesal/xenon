// *********************************************************************************************************************
// *                              Попытка очистки сценария аэропорта от сторонних объектов.                            *
// *                          А то все вкусные стоянки - уже заняты и нам ничего не осталось :-)                       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 17:11 *
// *********************************************************************************************************************
#pragma once

#include <string>
#include <vector>

#include "XPLMScenery.h"

namespace xenon {
    
    class AirportCleaner {
        
        public:
            
            AirportCleaner();
            ~AirportCleaner() = default;
            
            void clean();
            
        protected:
            
        private:
            
            std::vector< std::string > __names;
                        
    };
    
}; // namespace xenon
