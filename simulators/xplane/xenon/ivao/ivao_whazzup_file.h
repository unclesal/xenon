// *********************************************************************************************************************
// *                                   Файл состояния IVAO (кто где есть и в каком качестве).                          *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 mar 2020 at 13:44 *
// *********************************************************************************************************************
#pragma once

#ifdef IVAO

#include "ivao_periodically_updated_file.h"

namespace xenon {
    class IVAOWhazzupFile : public IVAOPeriodicallyUpdatedFile {
        public:
            IVAOWhazzupFile();
            ~IVAOWhazzupFile() = default;
            virtual void read() override;
            virtual std::string request_from_web() override;
        protected:
    };
};

#endif // IVAO
