#pragma once

#include <algorithm>
#include <numeric>
#include <tuple>
#include <optional>
#include <memory>

#include "transport_catalogue.h"
#include "graph.h"
#include "router.h"

using namespace transport_catalogue::catalogue;
using namespace graph;


class TransportRouter {
public:
    TransportRouter();
    explicit TransportRouter (TransportCatalogue& catalogue);

    struct TREdge{
        std::string bus_name;
        size_t span;
        domain::Stop* from;
        domain::Stop* to;
        double weight;
    };

    struct TRRouteInfo{
        double weight;
        std::vector<TREdge> edges;
    };

    std::optional<TRRouteInfo> BuildRoute(std::string_view from, std::string_view to);

private:
    std::vector<double> GetTimeBetweenStops(const domain::Bus&bus) const;

    template <typename IterIt>
    void CalculateEdges(const domain::Bus& bus, IterIt first, IterIt last);
    void CreateGraph();
private:
    transport_catalogue::catalogue::TransportCatalogue& catalogue_;
    graph::DirectedWeightedGraph<double> catalogue_graph_;
    std::vector<TREdge> edges_;
    std::vector<std::string_view> stop_names_;
    std::unique_ptr<graph::Router<double>> router_ = nullptr;
};


template <typename IterIt>
void TransportRouter::CalculateEdges(const domain::Bus& bus, IterIt first, IterIt last) {
    double time = catalogue_.GetBusWaitTime();
    auto size = distance(first, last);
    for(size_t i = 0; i < size; ++i) {
        for (size_t j = i + 1; j <= size; ++j) {
            double edge_weight = std::accumulate(first + i, first + j, time);
            auto span = j - i;
            edges_.push_back(TREdge{bus.name, span, bus.stops[i], bus.stops[j], edge_weight});
        }
    }
}
