#include "request_handler.h"

namespace request_handler{

RequestHandler::RequestHandler(const TransportCatalogue& catalogue):
    catalogue_(catalogue){}

std::optional<const std::set<RequestHandler::BusPtr>*>  RequestHandler::GetBusesByStop(const std::string& stop_name) const{
    if (catalogue_.FindStop(stop_name) != nullptr){
        return catalogue_.GetBusesForStop(catalogue_.FindStop(stop_name));
    }
    return std::nullopt;
}

//    BusStat = std::unordered_map<std::string, double>
std::optional<RequestHandler::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const{
    RequestHandler::BusStat answer;
    domain::Bus* bus_ptr = catalogue_.FindBus(bus_name);
    if (bus_ptr != nullptr){
        answer = catalogue_.GetBusStat(bus_name);
        return answer;
    }
    return std::nullopt;
}

} // namespace request_handler
