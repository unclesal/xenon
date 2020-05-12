// *********************************************************************************************************************
// *                                             Граф путей аэропорта.                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 may 2020 at 15:34 *
// *********************************************************************************************************************

#include <string>

#include "airport_network.h"
#include "xplane_utilities.h"

using namespace xenon;
using namespace std;
using namespace boost;

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                                Конструктор                                                        *
// *                                                                                                                   *
// *********************************************************************************************************************

AirportNetwork::AirportNetwork() {
};

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Добавить узел из формата apt.dat                                             *
// *                                                                                                                   *
// *********************************************************************************************************************

void AirportNetwork::add_apt_dat_node( const string &line, const vector<string> &contents ) {
    AirportNetwork::node_t node;
    auto latitude = stod( contents[1] );
    auto longitude = stod( contents[2] );
    node.location = boost_location_t( latitude, longitude, 0.0 );
    node.usage = contents[3];
    node.xp_id = stoi( contents[4] );
    if (contents.size() >= 6 ) {
        int pos = line.find( contents[5] );
        node.name = line.substr( pos );
    }
    boost::add_vertex( node, __graph ); // .m_vertices.push_back(node);
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                      Добавить ребро из формата apt.dat                                            *
// *                                                                                                                   *
// *********************************************************************************************************************

void AirportNetwork::add_apt_dat_edge(int i_type, const string &line, const vector<string> &contents) {

    // Очищаем дескрипторы последних добавленных ребер, прямого и обратного.
    __last_added_direct_edge = graph_t::edge_descriptor();
    __last_added_reverse_edge = graph_t::edge_descriptor();

    // Начальный и конечный узлы данного ребра.
    auto xp_start_id = stoi( contents[1] );
    auto xp_end_id = stoi( contents[2] );

    auto node_start = get_node_by_xp_id( xp_start_id );
    auto node_end = get_node_by_xp_id( xp_end_id );
    // При попытке получить узлы по их идентификатору возможно
    // исключение range_error - ну и ладно, пусть себе идет выше.
    edge_t edge;
    i_type == 1202 ? edge.vehicles = VT_AIRCRAFTS : edge.vehicles = VT_GROUND_VEHICLES;
    edge.xp_start_id = xp_start_id;
    edge.xp_end_id = xp_end_id;

    auto direction = contents[3];
    if ( contents.size() >= 5 ) {
        // Он, оказывается, тоже не является обязательным.
        auto xp_edge_type = contents[4];
        if ( xp_edge_type == "taxiway") edge.way_type = WAY_TAXIWAY;
        else if ( xp_edge_type == "runway" ) edge.way_type = WAY_RUNWAY;
        else throw runtime_error("AirportNetwork::add_apt_dat_edge, unhandled type " + xp_edge_type );

        if ( contents.size() >= 6 ) {
            auto pos = line.rfind( contents[ 5 ] );
            edge.name = line.substr( pos );
        }
    }

    // Прямая дуга будет в любом случае.
    auto direct_arc = add_edge( node_start, node_end, __graph );
    __last_added_direct_edge = direct_arc.first;
    __graph[ __last_added_direct_edge ] = edge;

    // Возможно, что есть и обратная дуга тоже.
    if ( direction == "twoway" ) {
        auto reverse_arc = add_edge( node_end, node_start, __graph );
        __last_added_reverse_edge = reverse_arc.first;
        __graph[ __last_added_reverse_edge ] = edge;
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                               Добавить активную зону ребра из формата apt.dat                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

void AirportNetwork::add_apt_dat_active_zone( const string &line, const vector<string> &contents) {

    active_zone_t az;
    az.classification = contents[1];
    az.runways = contents[2];
    // Здесь возможны ошибки. Причем - раздельно, по прямой и по обратной дуге.
    try {
        __graph[ __last_added_direct_edge ].active_zones.push_back( az );
    } catch ( std::range_error const & re ) {
        XPlaneUtilities::log("AirportNetwork::add_active_zone, range error for direct arc.");
    }

    try {
        __graph[ __last_added_reverse_edge ].active_zones.push_back( az );
    } catch ( std::range_error const & re ) {
        XPlaneUtilities::log("AirportNetwork::add_active_zone, range error for reverse zone.");
    }

}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Получить узел по его идентификатору из X-Plane                                     *
// *                                                                                                                   *
// *********************************************************************************************************************

AirportNetwork::graph_t::vertex_descriptor  AirportNetwork::get_node_by_xp_id(const int &xp_id) {
    graph_t::vertex_iterator vi, vi_end, next;
    for ( tie(vi, vi_end) = vertices( __graph ); vi != vi_end; ++vi) {
        graph_t::vertex_descriptor it = * vi;
        if ( __graph[ it ].xp_id == xp_id ) return it;
    }
    return graph_t::vertex_descriptor();
}
