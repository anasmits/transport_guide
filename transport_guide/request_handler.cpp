#include "request_handler.h"
#include "json_builder.h"

namespace request_handler{

    using namespace transport_catalogue::catalogue;
    using namespace renderer;
    using namespace json_reader;

namespace rh_detail{
    void CheckJsonRequestIsValid(const json::Dict& request_dict){
       // using namespace request_handler;

        if (request_dict.empty()){
            throw std::out_of_range("Request is empty"s);
        }
        if (request_dict.find("base_requests"s) == request_dict.end()){
            throw std::invalid_argument("base_requests are invalid"s);
        }
        if (request_dict.find("render_settings"s) == request_dict.end()){
            throw std::invalid_argument("render_settings are invalid"s);
        }
        if (request_dict.find("stat_requests"s) == request_dict.end()){
            throw std::invalid_argument("stat_requests are invalid"s);
        }
        if (request_dict.find("routing_settings"s) == request_dict.end()){
            throw std::invalid_argument("routing_settings are invalid"s);
        }
    }

} // namespace rh_detail

RequestHandler::RequestHandler(){
    catalogue_ptr = TransportCatalogue::GetInstance();
    renderer_ptr = std::make_unique<MapRenderer>(MapRenderer());
    json_reader_ptr = std::make_unique<JSONReader>(JSONReader(*catalogue_ptr));
    tr_router_ptr = std::make_unique<TransportRouter>(TransportRouter(*catalogue_ptr));
}

std::optional<const std::set<RequestHandler::BusPtr>*>  RequestHandler::GetBusesByStop(const std::string& stop_name) const{
    if (catalogue_ptr->FindStop(stop_name) != nullptr){
        return catalogue_ptr->GetBusesForStop(catalogue_ptr->FindStop(stop_name));
    }
    return std::nullopt;
}

RequestHandler& RequestHandler::LoadJson(std::istream& input){
    using namespace json;

    requests_dict = json_reader_ptr->LoadInput(input);
    rh_detail::CheckJsonRequestIsValid(requests_dict);
    return *this;
}

void RequestHandler::ParseAndProcessQueries(std::ostream& output){
    for (const auto& request : requests_dict){
        if (request.first == "base_requests") {
            json_reader_ptr->FillTransportCatalogue(request.second.AsArray());
        } else if (request.first == "stat_requests"){
            ExecuteStatRequest(request.second.AsArray(), output);
        } else if (request.first == "render_settings"){
            renderer_ptr->SetSettings(json_reader_ptr->ParseRenderSettingsRequests(request.second.AsDict()));
        } else if (request.first == "routing_settings") {
            json_reader_ptr->ParseRoutingSettingsRequest(request.second.AsDict());
        }
    }
}

void RequestHandler::ExecuteStatRequest(const json::Array& stat_requests, std::ostream& output) const{
    json::Array stat_answer;

    for (size_t i = 0; i < stat_requests.size(); ++i) {
        if (!stat_requests[i].AsDict().empty()){
            json::Dict request_answer;
            if (stat_requests[i].AsDict().at("type"s).AsString() == "Stop"s){
                GetStopStat(stat_requests[i].AsDict(), request_answer);
            } else if (stat_requests[i].AsDict().at("type"s).AsString() == "Bus"s){
                GetBusStat(stat_requests[i].AsDict(), request_answer);
            } else if (stat_requests[i].AsDict().at("type"s).AsString() == "Map"s) {
                GetMapStat(stat_requests[i].AsDict(), request_answer);
            } else if (stat_requests[i].AsDict().at("type"s).AsString() == "Route"s){
                GetRouteStat(stat_requests[i].AsDict(), request_answer);
            }else {
                throw std::invalid_argument("stat_requests type is invalid"s);
            }
            stat_answer.emplace_back(request_answer);
        }
    }

    json_reader_ptr->SendOutput(stat_answer, output);

}

void RequestHandler::GetStopStat(const json::Dict& stat_requests, json::Dict& request_answer) const{
    std::pair<int, std::string> stop_id_name = json_reader_ptr->ParseStatRequest(stat_requests);
    const auto buses_ptr = GetBusesByStop(stop_id_name.second);
    auto request_id = stop_id_name.first;

    if (buses_ptr == std::nullopt){
        request_answer = json_reader::NotFound(request_id).ToJson();
        return;
    } else {
        std::set<std::string> b;
        if (buses_ptr.value() == nullptr){
            request_answer = json_reader::Stop(request_id, b).ToJson();
            return;
        }

        for(const auto& bus : *buses_ptr.value()){
            b.emplace(bus->name);
        }
        request_answer = json_reader::Stop(request_id, b).ToJson();
        return;
    }
}

json_reader::Bus::BusStat RequestHandler::ConvertBusStat(const domain::Bus* bus_ptr) const{
    json_reader::Bus::BusStat bus_stat;
    auto busptr_to_geo_m_curv = catalogue_ptr->GetBusPtrStat(bus_ptr);

    bus_stat.curvature = std::get<2>(busptr_to_geo_m_curv);
    bus_stat.route_length = std::get<1>(busptr_to_geo_m_curv);
    bus_stat.stop_count = bus_ptr->stops.size();

    std::unordered_set<domain::Stop*> unique_stops = {bus_ptr->stops.begin(), bus_ptr->stops.end()};
    bus_stat.unique_stop_count = unique_stops.size();

    return bus_stat;
}

void RequestHandler::GetBusStat(const json::Dict& stat_requests, json::Dict& request_answer) const{
    std::pair<int, std::string> bus_id_name = json_reader_ptr->ParseStatRequest(stat_requests);
    auto request_id = bus_id_name.first;
    auto bus_name = bus_id_name.second;

    auto bus_ptr = catalogue_ptr->FindBus(bus_name);
    if (bus_ptr == nullptr || bus_ptr->stops.empty()){
        request_answer = json_reader::NotFound(request_id).ToJson();
    } else {
        request_answer = json_reader::Bus(request_id, ConvertBusStat(bus_ptr)).ToJson();
    }
}

svg::Document RequestHandler::RenderMap() const{
    auto buses_ptr = catalogue_ptr->GetBusesPtr();
    return renderer_ptr->RenderMap(buses_ptr);
}

void RequestHandler::GetMapStat(const json::Dict& stat_request, json::Dict& request_answer) const{
    int request_id = stat_request.at("id"s).AsInt();
    svg::Document document = RenderMap();
    std::ostringstream out_strstm;
    document.Render(out_strstm);
    request_answer = json_reader::Map(request_id, out_strstm.str()).ToJson();
}



void RequestHandler::GetRouteStat(const json::Dict& stat_request, json::Dict& request_answer) const{
    int request_id = stat_request.at("id"s).AsInt();
    auto from = stat_request.at("from"s).AsString();
    auto to = stat_request.at("to"s).AsString();

    if (from == to){
        request_answer = json_reader::RouteQuery(request_id, 0).ToJson();
        return;
    }

    auto route = tr_router_ptr->BuildRoute(from, to);

    if (route == std::nullopt){
        request_answer = json_reader::NotFound(request_id).ToJson();
        return;
    }

    json_reader::RouteQuery result(request_id, route.value().weight);
    for(const auto& item : route.value().edges){
        auto item1 =  new json_reader::RouteWait(request_id, item.from->name, catalogue_ptr->GetBusWaitTime());
        auto item2 = new json_reader::RouteMove(request_id, item.bus_name, item.span, item.weight - catalogue_ptr->GetBusWaitTime());
        result.AddItem(item1);
        result.AddItem(item2);
    }

    request_answer = result.ToJson();
}



TransportCatalogue& RequestHandler::GetCatalogue() {
    return *catalogue_ptr;
}


} // namespace request_handler


