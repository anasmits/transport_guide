#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include <string_view>
#include <vector>
#include <set>

#include "geo.h"

namespace transport_catalogue{
namespace catalogue{

const double TOL = 0.000001;

struct Stop{
    Stop();
    Stop(std::string name, double lat, double lng)
        : name(std::move(name))
        , coordinates({std::move(lat), std::move(lng)}){};
    Stop(std::string name, Coordinates coordinates)
        : name(std::move(name))
        , coordinates(std::move(coordinates)){};

    bool operator==(const Stop& other) const{
        return (name == other.name)
                && (std::abs(coordinates.lat - other.coordinates.lat) < TOL)
                && (std::abs(coordinates.lng - other.coordinates.lng) < TOL);
    };

    std::string name;
    Coordinates coordinates;
};

using StopPtrDistance = std::pair<Stop*, Stop*>;
inline size_t MakePairTwoStop(const StopPtrDistance& stops) {
    Stop* first = stops.first;
    Stop* second = stops.second;
    size_t hs1 = std::hash<std::string>{}(first->name)          * 37;
    size_t hs2 = std::hash<std::string>{}(second->name)         * 37*37;
    size_t hd11 = std::hash<double>{}(first->coordinates.lat)   * 37*37*37;
    size_t hd12 = std::hash<double>{}(first->coordinates.lng)   * 37*37*37*37;
    size_t hd21 = std::hash<double>{}(second->coordinates.lat)  * 37*37*37*37*37;
    size_t hd22 = std::hash<double>{}(second->coordinates.lng)  * 37*37*37*37*37*37;
    return hs1 + hs2 + hd11 + hd22 + hd12 + hd21;
}

struct StopPtrDistanceHasher{
    size_t operator()(const StopPtrDistance& two_stops){
        return MakePairTwoStop(two_stops);
    };
};

struct Bus{
    Bus(){
        name = {};
        stops = {};
        circle_rout = false;
    };
    Bus(std::string name, std::vector<Stop*> stops, bool rout_type)
        : name(std::move(name))
        , stops(std::move(stops))
        , circle_rout(std::move(rout_type)){};

    bool operator<(const Bus& other){
        return name < other.name;
    }
    bool operator==(const Bus& other){
        return name == other.name;
    }

    std::string name;
    std::vector<Stop*> stops;
    bool circle_rout;
};

class TransportCatalogue
{    
public:
    TransportCatalogue();
    ~TransportCatalogue();

    TransportCatalogue(const TransportCatalogue&) = delete;
    TransportCatalogue operator=(const TransportCatalogue&) = delete;

    void AddStop(const Stop& stop);
    void AddBus(const Bus& bus);

    Stop* FindStop(const std::string stop_name) const;
    Bus* FindBus(const std::string bus_name) const;

    std::string GetBusInfo(const std::string& bus_name) const;
    std::string GetStopInfo(const std::string& stop_name) const;
    double CalculateRouteLength(const Bus* bus);

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop;
    std::unordered_map<std::string_view, Bus*> busname_to_bus;
    std::unordered_map<size_t, double> stops_to_distance;
    std::unordered_map<std::string*, double> route_length;
    std::unordered_map<Stop*, std::set<std::string_view>> stopptr_to_busnames;
};


namespace detail{
}
} // namespace catalogue
} // namespace transport_cataloge
