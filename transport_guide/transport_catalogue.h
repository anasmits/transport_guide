#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include <string_view>
#include <vector>
#include <set>
#include <tuple>
#include <iostream>

#include "geo.h"

namespace transport_catalogue{
namespace catalogue{

const double TOL = 0.000001;

using namespace std::literals;

struct Stop{
    Stop() = default;
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

    std::string name = ""s;
    Coordinates coordinates = {0, 0};
};

using PairStopPtr = std::pair<Stop*, Stop*>;
using ConstPairStopPtr = std::pair<const Stop*, const Stop*>;

struct hash_pair_stopptr{
    size_t operator()(const PairStopPtr& p) const{
        auto hash1 = std::hash<const void*>{}(p.first);
        auto hash2 = std::hash<const void*>{}(p.second);
        return hash1*37. + hash2*37.*37.;
    }
};

struct hash_constpair_stopptr{
    size_t operator()(const ConstPairStopPtr& p) const{
        auto hash1 = std::hash<const void*>{}(p.first);
        auto hash2 = std::hash<const void*>{}(p.second);
        return hash1*37. + hash2*37.*37.;
    }
};


class StopPtr {
public:
    StopPtr() = default;
    explicit StopPtr(std::string name){
        raw_ptr_ = new Stop(name, {});
    }
    explicit StopPtr(std::string name, Coordinates coordinates){
        raw_ptr_ = new Stop(name, coordinates);
    }
    explicit StopPtr(Stop* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    StopPtr(const StopPtr&) = delete;
    StopPtr& operator=(const StopPtr&) = delete;

    StopPtr(StopPtr&& other){
        raw_ptr_ = std::exchange(other.raw_ptr_, nullptr);
    }

    StopPtr& operator=(StopPtr&& other){
        if (this != &other) {
            std::swap(other.raw_ptr_, raw_ptr_);
            return *this;
        }
    }

    ~StopPtr() {
        delete raw_ptr_;
    }

    [[nodiscard]] Stop* Release() noexcept {
        return std::exchange(raw_ptr_, nullptr);
    }

    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    Stop* Get() const noexcept {
        return raw_ptr_;
    }

    const Stop* GetConst() const noexcept {
        return raw_ptr_;
    }

    void swap(StopPtr& other) noexcept {
        std::swap(other.raw_ptr_, raw_ptr_);
    }

private:
    Stop* raw_ptr_ = nullptr;
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

using TupleDistance = std::tuple<double, int, double>;
struct hash_tuple_distance{
    size_t operator()(const TupleDistance& td) const{
        auto hash0 = std::hash<double>{}(std::get<0>(td));
        auto hash1 = std::hash<int>{}(std::get<1>(td));
        auto hash2 = std::hash<double>{}(std::get<2>(td));
        return hash0*37 + hash1*37*37 + hash2*37*37*37;
    }
};

using ConstPairBusPtrTuple = std::pair<const Bus*, TupleDistance>;
struct hash_constpair_busptr_tuple{
    size_t operator()(const ConstPairBusPtrTuple& p) const{
        auto hash1 = std::hash<const void*>{}(p.first);
        auto hash2 = hash_tuple_distance{}(p.second);
        return hash1 + hash2*37;
    }
};

class TransportCatalogue
{    
public:
    TransportCatalogue();
    ~TransportCatalogue();

    TransportCatalogue(const TransportCatalogue&) = delete;
    TransportCatalogue operator=(const TransportCatalogue&) = delete;

    void AddStop(const Stop& stop);
    void AddStop(const Stop* stop);
    void AddBus(const Bus& bus);

    void SetBusForStops(const std::vector<Stop*>& stops, std::string_view bus_name);
    std::set<std::string_view> GetBusesForStop(Stop* stop) const;

    void SetDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to, int distance);
    int GetDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to) const;

    void SetGeoDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to, double distance);
    int GetGeoDistanceBetweenStops(const Stop* stop_from, const Stop* stop_to) const;

    Stop* FindStop(const std::string stop_name) const;
    Bus* FindBus(const std::string bus_name) const;

    std::string GetBusInfo(const std::string& bus_name) const;
    std::string GetStopInfo(const std::string& stop_name) const;

/*This functions must be checked then commented*/
    double CalculateGeoRouteLength(const Bus* bus) const;
    int CalculateRouteLength(const Bus* bus) const;
    double CalculateCurvature(double geo_distance, int m_distance) const;

private:
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;
    std::unordered_map<std::string_view, Stop*> stopname_to_stop;
    std::unordered_map<std::string_view, Bus*> busname_to_bus;
    std::unordered_map<ConstPairStopPtr, double, hash_constpair_stopptr> stops_to_distance_geo; //  with coordinates
    std::unordered_map<ConstPairStopPtr, int, hash_constpair_stopptr> stops_to_distance_m; // with distance in meters
    std::unordered_map<const Bus*, std::tuple<double, int, double>, std::hash<const void*>> busptr_to_geo_m_curv;
//    std::unordered_map<Bus*, double, std::hash<const void*>> geo_route_length;
//    std::unordered_map<Bus*, int, std::hash<const void*>> route_length;
//    std::unordered_map<Bus*, double, std::hash<const void*>> curvature;
    std::unordered_map<Stop*, std::set<std::string_view>> stopptr_to_busnames;

/*This functions must be checked then uncommented*/
//    double CalculateGeoRouteLength(const Bus* bus) const;
//    int CalculateRouteLength(const Bus* bus) const;
//    double CalculateCurvature(double geo_distance, int m_distance) const;

};


namespace detail{


} // namespace detail
} // namespace catalogue
} // namespace transport_cataloge
