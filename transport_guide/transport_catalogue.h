#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <string_view>
#include <vector>
#include <set>
#include <tuple>
#include <iostream>
#include <optional>

#include "domain.h"
#include "graph.h"
//#include "transport_router.h"


namespace transport_catalogue{
namespace catalogue{

using namespace domain;

// singleton
class TransportCatalogue
{    
private:
    TransportCatalogue();
    static TransportCatalogue* catalogue_instance_ptr;

public:
    ~TransportCatalogue();

    TransportCatalogue(const TransportCatalogue&) = delete;
    TransportCatalogue& operator=(const TransportCatalogue&) = delete;
    TransportCatalogue(TransportCatalogue&&) = default;


    static TransportCatalogue* GetInstance();

    friend class TransportRouter;


    void AddStop(const Stop& stop);
    void AddStop(const Stop* stop);
    void AddBus(const Bus& bus);

    void SetBusForStops(const std::vector<Stop*>& stops, std::string_view bus_name);
    const std::set<Bus*>* GetBusesForStop(Stop* stop) const;

    void SetDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to, int distance);
    int GetDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to) const;

    void SetGeoDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to, double distance);
    int GetGeoDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to) const;

    Stop* FindStop(const std::string_view stop_name) const;
    Bus* FindBus(const std::string_view bus_name) const;

    std::string GetBusInfo(const std::string& bus_name) const;
    std::string GetStopInfo(const std::string& stop_name) const;

//    std::unordered_map<std::string, double> GetBusStat(const std::string_view& bus_name) const;
    std::tuple<double, int, double> GetBusPtrStat(const Bus* bus_ptr) const;

    const std::deque<Bus>* GetBusesPtr() const;
    const std::deque<Stop>* GetStopsPtr() const;
    std::deque<Stop>* GetStopsPtr();

    void CalculateRouteAndCurvature();

    const size_t BUS_WAIT_TIME_MAX = 1000;
    const size_t BUS_WAIT_TIME_MIN = 1;
    const double BUS_VELOCITY_MAX = 1000;
    const double BUS_VELOCITY_MIN = 1;

    void SetBusVelocity(double vel);
    double GetBusVelocity();

    void SetBusWaitTime(size_t wait_time);
    size_t GetBusWaitTime();

    void MakeGraph();

    static size_t GetStopsCount();

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop;
    std::unordered_map<std::string_view, Bus*> busname_to_bus;
    std::unordered_map<ConstPairStopPtr, double, hash_constpair_stopptr> stops_to_distance_geo; //  with coordinates
    std::unordered_map<ConstPairStopPtr, int, hash_constpair_stopptr> stops_to_distance_m; // with distance in meters
    std::unordered_map<const Bus*, std::tuple<double, int, double>, std::hash<const void*>> busptr_to_geo_m_curv;
    std::unordered_map<Stop*, std::set<Bus*>> stopptr_to_busptr;
    size_t bus_wait_time;
    double bus_velocity_mps;

    static size_t stops_count;
/*This functions must be checked then uncommented*/
    double CalculateGeoRouteLength(const Bus* bus) const;
    int CalculateRouteLength(const Bus* bus) const;
    double CalculateCurvature(double geo_distance, int m_distance) const;
};




namespace detail{

} // namespace detail
} // namespace catalogue
} // namespace transport_catalogue
