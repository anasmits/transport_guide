#pragma once

#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <memory>

#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"
#include "json_reader.h"
#include "domain.h"
#include "svg.h"
#include "graph.h"

namespace request_handler{

using namespace transport_catalogue::catalogue;

class RequestHandler /*Facade for TransportCatalogue, svg*/{
public:
    using BusPtr = domain::Bus*;
    using BusStat = std::unordered_map<std::string, double>;

    RequestHandler();

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusInfo(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через остановку
    std::optional<const std::set<BusPtr>*> GetBusesByStop(const std::string& stop_name) const;

    svg::Document RenderMap() const;

    RequestHandler& LoadJson(std::istream& input = std::cin);
    void ParseAndProcessQueries( std::ostream& output = std::cout);
    void ExecuteStatRequest(const json::Array& stat_requests, std::ostream& output)const;
    void GetStopStat(const json::Dict& stat_requests, json::Dict& request_answer) const;
    void GetBusStat(const json::Dict& stat_requests, json::Dict& request_answer) const;
    void GetMapStat(const json::Dict& stat_request, json::Dict& request_answer) const;
    void GetRouteStat(const json::Dict& stat_request, json::Dict& request_answer) const;

    TransportCatalogue& GetCatalogue();
private:
    json_reader::Bus::BusStat ConvertBusStat(const domain::Bus* bus_ptr) const;
private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник", "Визуализатор Карты" и "Парсер JSON"
    TransportCatalogue* catalogue_ptr; // singletone
    std::unique_ptr<renderer::MapRenderer> renderer_ptr;
    std::unique_ptr<json_reader::JSONReader> json_reader_ptr; // interface
    std::unique_ptr<TransportRouter> tr_router_ptr;

    json::Dict requests_dict;
};

} // namespace request_handler
