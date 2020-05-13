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
    auto way = usss.get_taxi_way_for_departure(location);

    // double dis1 = XPlane::distance( location, way[0] );
    // double dis2 = XPlane::distance( location, way[ way.size() - 1 ]);

    return 0;
}
