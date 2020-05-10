// *********************************************************************************************************************
// *                          Поток чтения состояния IVAO с использованием файла whazzup.txt IVAO API                  *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 mar 2020 at 13:32 *
// *********************************************************************************************************************
#pragma once

#ifdef IVAO

#include <thread>
#include <string>

using namespace std;
namespace xenon {

    void * whazzup_reader_thread(void * args);

}; // namespace xenon

#endif // IVAO
