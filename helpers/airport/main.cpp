#include <iostream>
#include <string>

#include "airport.h"

void parsed(string & icao_code ) {
    cout << "Parsed: " << icao_code << endl;
};

using namespace xenon;

int main() {

    Airport::set_has_been_parsed( parsed );
    Airport::read_all();
    Airport usss = Airport::get_by_icao("USSS");
    location_t location;

    location.latitude=56.749406;
    location.longitude=60.797907;
    location.altitude = usss.evalution_in_meters();

    location_t t1 = xenon::shift( location, -30, 180.0 );
    auto heading = xenon::bearing( location, t1 );
    auto distance = xenon::distance2d( location, t1 );

//    auto way = usss.get_taxi_way_for_departure(location);

//    cout << "way0 la=" << way.at(0).location.latitude << ", lo=" << way.at(0).location.longitude << endl;

//    location_t point1 {
//        .latitude = xenon::degrees_to_decimal(55, 45, 0, 'N'),
//        .longitude = xenon::degrees_to_decimal(37, 37, 0, 'E'),
//        .altitude = 0.0
//    };

//    location_t point2 {
//        .latitude = xenon::degrees_to_decimal(59, 53, 0, 'N'),
//        .longitude = xenon::degrees_to_decimal(30, 15, 0, 'E'),
//        .altitude = 0.0
//    };

//    auto distance = xenon::distance2d(
//        point1, point2
//    );

//    cout << to_string(distance) << endl;
    return 0;
}
