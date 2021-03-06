// *********************************************************************************************************************
// *     Некоторая штука (вообще-то "агент"), которая способна общаться с другими такими же "штуками" (агентами)       *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 26 may 2020 at 15:09 *
// *********************************************************************************************************************
#include "talker.h"
using namespace xenon;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

Talker::Talker( const talker_t & talker ) {
    __talker = talker;
}
