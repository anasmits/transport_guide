#pragma once

#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <memory>

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_reader.h"
#include "domain.h"
#include "svg.h"

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

    // Этот метод будет нужен в следующей части итогового проекта
    svg::Document RenderMap() const;

    void LoadJson(std::istream& input = std::cin, std::ostream& output = std::cout);
    void ExecuteStatRequest(const json::Array& stat_requests, std::ostream& output)const;
    void GetStopStat(const json::Dict& stat_requests, json::Dict& request_answer) const;
    void GetBusStat(const json::Dict& stat_requests, json::Dict& request_answer) const;
    void GetMapStat(const json::Dict& stat_request, json::Dict& request_answer) const;

private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник", "Визуализатор Карты" и "Парсер JSON"
    std::unique_ptr<TransportCatalogue> catalogue_ptr;
    std::unique_ptr<renderer::MapRenderer> renderer_ptr;
    std::unique_ptr<json_reader::JSONReader> json_reader_ptr;
};

} // namespace request_handler
