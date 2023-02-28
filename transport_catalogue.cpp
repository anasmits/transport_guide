#include "transport_catalogue.h"

#include <iostream>
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

void TransportCatalogue::AddBus(const Bus& bus){
    buses_.push_back(std::move(bus));
    busname_to_bus[buses_.back().name] = &buses_.back();
    route_length.insert(std::make_pair(&buses_.back().name, CalculateRouteLength(busname_to_bus[buses_.back().name])));
}

Stop* TransportCatalogue::FindStop(const std::string stop_name) const{
    if(stopname_to_stop.find(stop_name) == stopname_to_stop.end()){
        return nullptr;
    }
    return stopname_to_stop.at(stop_name);
}

Bus* TransportCatalogue::FindBus(const std::string bus_name) const {
    if(busname_to_bus.find(bus_name) == busname_to_bus.end()){
        return nullptr;
    }
    return busname_to_bus.at(bus_name);
}

double TransportCatalogue::CalculateRouteLength(const Bus* bus){
    double route_length = 0;
    for(size_t i = 0; i < bus->stops.size()-1; ++i){
        Stop* first_stop = bus->stops[i];
        Stop* second_stop = bus->stops[i+1];
        double distance = ComputeDistance(first_stop->coordinates, second_stop->coordinates);
        stops_to_distance[MakePairTwoStop({first_stop, second_stop})] = distance;
        stops_to_distance[MakePairTwoStop({second_stop, second_stop})] = distance;
        route_length += distance;
    }
    return route_length;
}

std::string TransportCatalogue::GetBusInfo(const std::string& bus_name) const {
    using namespace std::literals;

    const Bus* bus = FindBus(bus_name);
    if (bus == nullptr){
        return "Bus "s + bus_name + ": not found"s;
    }
    std::unordered_set<Stop*> unique_stops = {bus->stops.begin(), bus->stops.end()};
    std::ostringstream out;
    out << "Bus " << bus_name << ": "
        << bus->stops.size() << " stops on route, "s
        << unique_stops.size() << " unique stops, "s
        << std::setprecision(6) << route_length.at(const_cast<std::string*>(&bus->name)) << " route length"s;
    return out.str();
}