#include "transport_router.h"

TransportRouter::TransportRouter(TransportCatalogue& catalogue) : catalogue_(catalogue){
}

std::vector<double>  TransportRouter::GetTimeBetweenStops(const domain::Bus&bus) const{
    auto stops = bus.stops;

    double velocity = catalogue_.GetBusVelocity();

    std::vector<double> time_from_stop_to_stop;
    for (size_t i = 0; i < stops.size()-1; ++i){
        time_from_stop_to_stop.emplace_back(catalogue_.GetDistanceBetweenStops(stops[i], stops[i+1]) / velocity);
    }
    return time_from_stop_to_stop;
}

std::tuple<domain::Bus, const domain::Bus> MakeTwoRoutesFromOne(const domain::Bus& bus){
    auto bus_first = domain::Bus(bus.name, {bus.stops.begin(), next(bus.stops.begin(), bus.stops.size()/2)+1}, false);
    auto bus_second = domain::Bus(bus.name, {next(bus.stops.begin(), bus.stops.size()/2), bus.stops.end()}, false);
    return std::make_tuple(bus_first, bus_second);
}


void TransportRouter::CreateGraph(){
    for(const auto& stop : *catalogue_.GetStopsPtr()){
        stop_names_.emplace_back(stop.name);
    }

    auto bus_ptr = catalogue_.GetBusesPtr();

    for (const auto& bus : *bus_ptr){

        auto time_stop_to_stop = GetTimeBetweenStops(bus);

        if (time_stop_to_stop.size() == 1){
            edges_.emplace_back(TREdge{bus.name, 1, bus.stops[0], bus.stops[1], time_stop_to_stop[0] });
        } else {
            if (bus.is_roundtrip){
                CalculateEdges(bus, time_stop_to_stop.begin(), time_stop_to_stop.end());
            } else {
                size_t middle = time_stop_to_stop.size()/2 ;
                auto it_middle = next(time_stop_to_stop.begin(), middle);
                auto [bus_first, bus_second] = MakeTwoRoutesFromOne(bus);

                CalculateEdges(bus_first, time_stop_to_stop.begin(), it_middle);
                CalculateEdges(bus_second, next(time_stop_to_stop.begin(), middle), time_stop_to_stop.end());
            }
        }

    }

    catalogue_graph_ = graph::DirectedWeightedGraph<double>(static_cast<size_t>(stop_names_.size()));
    for (const auto& tr_edge : edges_){
        catalogue_graph_.AddEdge(Edge<double>{tr_edge.from->number, tr_edge.to->number, tr_edge.weight});
    }
}

std::optional<TransportRouter::TRRouteInfo> TransportRouter::BuildRoute(std::string_view from, std::string_view to){
    if (!router_) {
        CreateGraph();
        router_ = std::make_unique<Router<double>>(Router<double>(catalogue_graph_));
    }

    graph::VertexId from_vertex = distance(stop_names_.begin(), find(stop_names_.begin(), stop_names_.end(), from));
    graph::VertexId to_vertex = distance(stop_names_.begin(), find(stop_names_.begin(), stop_names_.end(), to));

    auto route_info = router_->BuildRoute(from_vertex, to_vertex);
    if (route_info == std::nullopt){
        return std::nullopt;
    }

    std::vector<TREdge> tr_edges;
    for(const auto& item : route_info.value().edges){
        tr_edges.push_back(edges_[item]);
    }
    TRRouteInfo info{route_info.value().weight, std::move(tr_edges)};
    return info;
}