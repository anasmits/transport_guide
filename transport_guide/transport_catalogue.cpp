#include "transport_catalogue.h"

#include <iomanip>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <unordered_set>

using namespace transport_catalogue::catalogue;

TransportCatalogue::TransportCatalogue(){}

TransportCatalogue::~TransportCatalogue(){}

void TransportCatalogue::AddStop(const Stop& stop){
    stops_.push_back(std::move(stop));
    stopname_to_stop[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddStop(const Stop* stop){
    stops_.push_back(std::move(*stop));
    stopname_to_stop[stops_.back().name] = &stops_.back();
}

void TransportCatalogue::AddBus(const Bus& bus){
    buses_.push_back(std::move(bus));
    busname_to_bus[buses_.back().name] = &buses_.back();
    SetBusForStops(buses_.back().stops, buses_.back().name);
}

void TransportCatalogue::SetBusForStops(const std::vector<Stop*>& stops, std::string_view bus_name){
    for(auto stop : stops){
        stopptr_to_busptr[stopname_to_stop.at(stop->name)].insert(FindBus(bus_name));
    }
}

const std::set<Bus*>* TransportCatalogue::GetBusesForStop(Stop* stop) const{
    if(stopptr_to_busptr.find(stop) != stopptr_to_busptr.end()){
        return &stopptr_to_busptr.at(stop);
    }
    return nullptr;
}

void TransportCatalogue::SetDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to, int distance){
    std::pair<const Stop*, const Stop*> stops = std::make_pair(stop_from, stop_to);
    stops_to_distance_m[stops] = distance;
}

int TransportCatalogue::GetDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to) const{
    std::pair<const Stop*, const Stop*> stops12 = std::make_pair(stop_from, stop_to);
    std::pair<const Stop*, const Stop*> stops21 = std::make_pair(stop_to, stop_from);
    if(stops_to_distance_m.find(stops12) != stops_to_distance_m.end()){
        return stops_to_distance_m.at(stops12);
    } else if(stops_to_distance_m.find(stops21) != stops_to_distance_m.end()){
        return stops_to_distance_m.at(stops21);
    } else{
        using namespace std::literals;
           throw std::out_of_range("Distance is not found"s);
    }
}

void TransportCatalogue::SetGeoDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to, double distance){
    stops_to_distance_geo[{stop_from, stop_to}] = distance;
    stops_to_distance_geo[{stop_to, stop_from}] = distance;
}

int TransportCatalogue::GetGeoDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to) const{
    std::pair<const Stop*, const Stop*> stops = std::make_pair(stop_from, stop_to);
    if(stops_to_distance_geo.find(stops) != stops_to_distance_geo.end()){
        return stops_to_distance_geo.at(stops);
    }else {
        using namespace std::literals;
        throw std::out_of_range("Geo distance for stops is not found"s);
    }
}

Stop* TransportCatalogue::FindStop(const std::string_view stop_name) const{
    if(stopname_to_stop.find(stop_name) == stopname_to_stop.end()){
        return nullptr;
    }
    return stopname_to_stop.at(stop_name);
}

Bus* TransportCatalogue::FindBus(const std::string_view bus_name) const {
    if(busname_to_bus.find(bus_name) == busname_to_bus.end()){
        return nullptr;
    }
    return busname_to_bus.at(bus_name);
}

double TransportCatalogue::CalculateGeoRouteLength(const Bus* bus) const{
    double route_length = 0;
    for(size_t i = 0; i < bus->stops.size()-1; ++i){
        Stop* first_stop = bus->stops[i];
        Stop* second_stop = bus->stops[i+1];
        double distance = ComputeDistance(first_stop->coordinates, second_stop->coordinates);
//        SetGeoDistanceBetweenStops(first_stop, second_stop, distance);
        route_length += distance;
    }
    return route_length;
}

int TransportCatalogue::CalculateRouteLength(const Bus* bus) const{
    int route_length = 0;
    for(size_t i = 0; i < bus->stops.size()-1 ; ++i){
        const Stop* first_stop = stopname_to_stop.at(bus->stops[i]->name);
        const Stop* second_stop = stopname_to_stop.at(bus->stops[i+1]->name);
        route_length += GetDistanceBetweenStops(first_stop, second_stop);
    }
    return route_length;
}

double TransportCatalogue::CalculateCurvature(double geo_distance, int m_distance) const{
    if(geo_distance <= 0){
       throw std::out_of_range ("CalculateCurvature:: The geo distance is 0. Cannot devide by 0.");
    }
    return static_cast<double>(m_distance) / geo_distance;
}

void TransportCatalogue::CalculateRouteAndCurvature(){
    for(const auto& [busname, busptr] : busname_to_bus){
        double geo = CalculateGeoRouteLength(busptr);
        int m = CalculateRouteLength(busptr);
        double curv =  CalculateCurvature(geo, m);
        busptr_to_geo_m_curv.insert(std::make_pair(busptr, std::make_tuple(geo, m, curv)));
    }
}

std::string TransportCatalogue::GetBusInfo(const std::string& bus_name) const{
    using namespace std::literals;

    Bus* bus = FindBus(bus_name);
    if (bus == nullptr){
        return "Bus "s + bus_name + ": not found"s;
    }
    std::unordered_set<Stop*> unique_stops = {bus->stops.begin(), bus->stops.end()};


//    double geo = CalculateGeoRouteLength(bus);
//    int m = CalculateRouteLength(bus);
//    double curv =  CalculateCurvature(geo, m);
//    busptr_to_geo_m_curv.insert(std::make_pair(&buses_.back(), std::make_tuple(geo, m, curv)));

    std::ostringstream out;
    out << "Bus " << bus_name << ": "
        << bus->stops.size() << " stops on route, "s
        << unique_stops.size() << " unique stops, "s
        << std::setprecision(6) << std::get<1>(busptr_to_geo_m_curv.at(bus)) << " route length, "s
        << std::setprecision(6) << std::get<2>(busptr_to_geo_m_curv.at(bus)) << " curvature"s;
//        << std::setprecision(6) << m << " route length, "s
//        << std::setprecision(6) << curv << " curvature"s;
    return out.str();
}

std::string TransportCatalogue::GetStopInfo(const std::string &stop_name) const
{
    using namespace std::literals;

    auto stop = FindStop(stop_name);
    if(stop == nullptr){
        return "Stop "s + stop_name + ": not found"s;
    }
    if(stopptr_to_busptr.find(stop) == stopptr_to_busptr.end()){
        return "Stop "s + stop_name + ": no buses"s;
    }
    std::ostringstream out;
    out << "Stop " << stop_name << ": buses";
    for(const auto& busptr: stopptr_to_busptr.at(stop)){
        out << " "s << busptr->name;
    }
    return out.str();
}

std::unordered_map<std::string, double> TransportCatalogue::GetBusStat(const std::string_view& bus_name) const{
    std::unordered_map<std::string, double> result;
    auto bus = busname_to_bus.at(bus_name);
    result["curvature"s] = std::get<2>(busptr_to_geo_m_curv.at(bus));
    result["route_length"s] = std::get<1>(busptr_to_geo_m_curv.at(bus));
    result["stop_count"s] = bus->stops.size();
    std::unordered_set<Stop*> unique_stops = {bus->stops.begin(), bus->stops.end()};
    result["unique_stop_count"s] = unique_stops.size();
    return result;
}

const std::deque<Bus>* TransportCatalogue::GetBusesPtr() const {
    return &buses_;
}

const std::deque<Stop>* TransportCatalogue::GetStopsPtr() const{
    return &stops_;
}
