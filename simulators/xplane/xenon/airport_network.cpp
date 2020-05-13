// *********************************************************************************************************************
// *                                             Граф путей аэропорта.                                                 *
// *                                                                                                                   *
// * Eugene G. Sysoletin <e.g.sysoletin@gmail.com>                                        Created 10 may 2020 at 15:34 *
// *********************************************************************************************************************

#include <string>
#include <deque>
#include <boost/property_map/function_property_map.hpp>

#include "constants.h"
#include "airport_network.h"
#include "xplane.hpp"

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
    node.location.latitude = stod( contents[1] );
    node.location.longitude = stod( contents[2] );
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

    // Вычисление дистанции между узлами - один раз при создании дуги. Она все
    // равно не меняется же, поэтому высчитывать дистанцию каждый раз - грешно.
    auto elem_node_start = __graph[ node_start ];
    auto elem_node_end = __graph[ node_end ];
    edge.distance = xenon::distance(elem_node_start.location, elem_node_end.location);

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
        XPlane::log("AirportNetwork::add_active_zone, range error for direct arc.");
    }

    try {
        __graph[ __last_added_reverse_edge ].active_zones.push_back( az );
    } catch ( std::range_error const & re ) {
        XPlane::log("AirportNetwork::add_active_zone, range error for reverse zone.");
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

// *********************************************************************************************************************
// *                                                                                                                   *
// *                            Получить все ребра данного узла, и входящие, и исходящие                               *
// *                                                                                                                   *
// *********************************************************************************************************************

vector<AirportNetwork::edge_t> AirportNetwork::get_edges_for( const graph_t::vertex_descriptor & node_descriptor ) {
    vector<edge_t> result;
    graph_t::edge_iterator i, end;
    for (boost::tie(i, end) = edges( __graph ); i != end; ++ i) {
        auto e_descriptor = * i;
        graph_t::vertex_descriptor source_descriptor = source( e_descriptor, __graph );
        graph_t::vertex_descriptor target_descriptor = target( e_descriptor, __graph );
        if (( source_descriptor == node_descriptor ) || ( target_descriptor == node_descriptor )) {
            edge_t edge = __graph[ e_descriptor ];
            result.push_back( edge );
        }
    }
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                Получить ближайший узел дорожки указанного типа.                                   *
// *                                                                                                                   *
// *********************************************************************************************************************

AirportNetwork::graph_t::vertex_descriptor AirportNetwork::get_nearest_node(
    const location_t & from, const way_type_t way_type
) {

    AirportNetwork::graph_t::vertex_descriptor result;
    double min_distance = FAR_AWAY;

    graph_t::vertex_iterator vi, vi_end, next;
    for ( tie(vi, vi_end) = vertices( __graph ); vi != vi_end; ++vi) {
        graph_t::vertex_descriptor node_descriptor = * vi;
        auto edges = get_edges_for( node_descriptor );
        for ( const auto & e : edges ) {
            if ((way_type == WAY_ANY) || ( e.way_type == way_type )) {
                // Обнаружено ребро с нужным нам типом. Смотрим на расстояние.
                node_t here_node = __graph[ node_descriptor ];
                double current_distance = distance(from, here_node.location);

                if ( current_distance <= min_distance ) {
                    min_distance = current_distance;
                    result = node_descriptor;
                }
            }
        }
    }
    return result;
}

// *********************************************************************************************************************
// *                                                                                                                   *
// *                                         Кратчайший путь по Dijkstra                                               *
// *                                                                                                                   *
// *********************************************************************************************************************

std::deque< AirportNetwork::graph_t::vertex_descriptor > AirportNetwork::shortest_path(
    const graph_t::vertex_descriptor & start_node_descriptor,
    const location_t & to_location
) {

    std::deque< graph_t::vertex_descriptor > path;

    auto end_node_descriptor = get_nearest_node( to_location, WAY_ANY );
    size_t visited;
    AirportNetwork::dij_visitor_t vis( end_node_descriptor, visited );

    auto null_vertex = xenon::AirportNetwork::graph_t::null_vertex();
    std::vector<boost::default_color_type>  colors(num_vertices( __graph ), boost::default_color_type{});
    std::vector<graph_t::vertex_descriptor> _pred(num_vertices( __graph ), null_vertex  );
    std::vector<double>                     _dist(num_vertices( __graph ), FAR_AWAY );
    auto predmap = _pred.data(); // interior properties: boost::get(boost::vertex_predecessor, g);
    auto distmap = _dist.data(); // interior properties: boost::get(boost::vertex_distance, g);

    // лямбда-функция для выдачи дистанции в качестве map'а.
    auto g = __graph;
    auto custom_distance = boost::make_function_property_map< graph_t::edge_descriptor >(
        [ & g ]( graph_t::edge_descriptor e ) {
            return g[ e ].distance;
        });

    try {

        boost::dijkstra_shortest_paths(
            __graph, start_node_descriptor, boost::visitor( vis ).
                color_map( colors.data() ).
                distance_map( distmap ).
                predecessor_map( predmap ).
                weight_map( custom_distance )
        );

    } catch ( AirportNetwork::dij_visitor_t::done const & ) {

        auto distance = distmap[ end_node_descriptor ];
        if ( distance != FAR_AWAY ) {

            for ( graph_t::vertex_descriptor current = end_node_descriptor;
                  current != xenon::AirportNetwork::graph_t::null_vertex()
                  && predmap[ current ] != current
                  && current != start_node_descriptor;
            ) {

                path.push_front( predmap[ current ] );
                current = predmap[ current ];
            }

        }
    }
    return path;
}
