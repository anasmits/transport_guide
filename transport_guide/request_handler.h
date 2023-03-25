#pragma once

#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <string>

#include "transport_catalogue.h"
#include "domain.h"
//#include "svg.h"

namespace request_handler{

using namespace transport_catalogue::catalogue;

class RequestHandler /*Facade for TransportCatalogue, svg*/{
public:
    using BusPtr = domain::Bus*;
    using BusStat = std::unordered_map<std::string, double>;

    explicit RequestHandler(const TransportCatalogue& catalogue);

    // Возвращает информацию о маршруте (запрос Bus)
    std::optional<BusStat> GetBusStat(const std::string_view& bus_name) const;

    // Возвращает маршруты, проходящие через остановку
    std::optional<const std::set<BusPtr>*> GetBusesByStop(const std::string& stop_name) const;

//    // Этот метод будет нужен в следующей части итогового проекта
//    svg::Document RenderMap() const;

//    // MapRenderer понадобится в следующей части итогового проекта
//    RequestHandler(const TransportCatalogue& db, const renderer::MapRenderer& renderer);


private:
    // RequestHandler использует агрегацию объектов "Транспортный Справочник" и "Визуализатор Карты"
    const TransportCatalogue& catalogue_;
//    const renderer::MapRenderer& renderer_;
};

} // namespace request_handler
