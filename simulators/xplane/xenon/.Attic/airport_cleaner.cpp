// *********************************************************************************************************************
// *                              Попытка очистки сценария аэропорта от сторонних объектов.                            *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 27 may 2020 at 17:11 *
// *********************************************************************************************************************


#include "airport_cleaner.h"
#include "xplane.hpp"


using namespace xenon;


void __cb__airport_cleaner__object_found( const char * file_path, void * ref ) {
    if ( ref ) XPLMUnloadObject( ref );
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                    Конструктор                                                    *
// *                                                                                                                   *
// *********************************************************************************************************************

AirportCleaner::AirportCleaner() {

    __names.push_back( "ruscenery/aircrafts/Yak-42-6.obj" );
    __names.push_back( "ruscenery/aircrafts/Yak-42-8.obj" );
    __names.push_back( "ruscenery/aircrafts/Tu-154-M.obj" );
    __names.push_back( "ruscenery/aircrafts/Tu-154-g.obj" );
    __names.push_back( "ruscenery/aircrafts/Tu-154.obj" );
    __names.push_back( "ruscenery/aircrafts/Il-96-1.obj" );
    __names.push_back( "ruscenery/aircrafts/B-737-800-4.obj" );    
    

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Метод очистки сценария                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AirportCleaner::clean() {
    
    XPLMLibraryEnumerator_f callback = __cb__airport_cleaner__object_found;
    for ( int i=0; i<__names.size(); ++ i ) {
        std::string path = __names.at(i);
        int result = XPLMLookupObjects( path.c_str(), 0, 0, callback, nullptr );
        if ( ! result ) XPlane::log("Not found for " + path);
    }
}
