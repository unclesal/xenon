// *********************************************************************************************************************
// *                                                   Сущностный фрейм                                                *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 05 jun 2020 at 09:50 *
// *********************************************************************************************************************
#pragma once

#include "abstract_frame.h"

namespace xenon {
    
    class EntityFrame : public AbstractFrame {
        public:
            EntityFrame();
            virtual ~EntityFrame() override = default;
    };
    
};
