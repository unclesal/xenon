// *********************************************************************************************************************
// *                                                   Файл статуса IVAO                                               *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 29 mar 2020 at 11:40 *
// *********************************************************************************************************************
#pragma once

#ifdef IVAO

#include <string>

#include "ivao_periodically_updated_file.h"

namespace xenon {
    class IVAOStatusFile: public xenon::IVAOPeriodicallyUpdatedFile {
        public:
            IVAOStatusFile();
            ~IVAOStatusFile() = default;
            virtual void read() override;
            
            std::string whazzup_url() {
                return __whazzup_url;
            };
            
            std::string zipped_whazzup_url() {
                return __zipped_whazzup_url;
            };
            
        private:
            std::string __whazzup_url;
            std::string __voice_url;
            std::string __zipped_whazzup_url;
    };
}; // namespace xenon

#endif // IVAO
