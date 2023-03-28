#include "request_handler.h"

namespace request_handler{

RequestHandler::RequestHandler(){
    catalogue_ptr = std::make_unique<transport_catalogue::catalogue::TransportCatalogue>(transport_catalogue::catalogue::TransportCatalogue());
    renderer_ptr = std::make_unique<renderer::MapRenderer>(renderer::MapRenderer());
    json_reader_ptr = std::make_unique<json_reader::JSONReader>(json_reader::JSONReader(*catalogue_ptr.get()));
}


std::optional<const std::set<RequestHandler::BusPtr>*>  RequestHandler::GetBusesByStop(const std::string& stop_name) const{
    if (catalogue_ptr->FindStop(stop_name) != nullptr){
        return catalogue_ptr->GetBusesForStop(catalogue_ptr->FindStop(stop_name));
    }
    return std::nullopt;
}

//    BusStat = std::unordered_map<std::string, double>
std::optional<RequestHandler::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_name) const{
    RequestHandler::BusStat answer;
    domain::Bus* bus_ptr = catalogue_ptr->FindBus(bus_name);
    if (bus_ptr != nullptr){
        answer = catalogue_ptr->GetBusStat(bus_name);
        return answer;
    }
    return std::nullopt;
}

void RequestHandler::LoadJson(std::istream& input, std::ostream& output){
    using namespace json;

    auto request_dict = json_reader_ptr->LoadInput(input);

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

    for (const auto& request : request_dict){
        if (request.first == "base_requests") {
            json_reader_ptr->FillTransportCatalogue(request.second.AsArray());
        } else if (request.first == "stat_requests"){
            ExecuteStatRequest(request.second.AsArray(), output);
        } else if (request.first == "render_settings"){
            renderer_ptr->SetSettings(json_reader_ptr->ParseRenderSettingsRequests(request.second.AsMap()));
        }
    }
}


void RequestHandler::ExecuteStatRequest(const json::Array& stat_requests, std::ostream& output) const{
    json::Array stat_answer;

    for (size_t i = 0; i < stat_requests.size(); ++i) {
        if (!stat_requests[i].AsMap().empty()){
            json::Dict request_answer;
            if (stat_requests[i].AsMap().at("type"s).AsString() == "Stop"s){
                GetStopStat(stat_requests[i].AsMap(), request_answer);
            } else if (stat_requests[i].AsMap().at("type"s).AsString() == "Bus"s){
                GetBusStat(stat_requests[i].AsMap(), request_answer);
            } else if (stat_requests[i].AsMap().at("type"s).AsString() == "Map"s) {
                GetMapStat(stat_requests[i].AsMap(), request_answer);
            } else {
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
    if (buses_ptr == std::nullopt){
        request_answer = {{"request_id"s, stop_id_name.first}, {"error_message"s, "not found"}};
    } else if (buses_ptr.value() == nullptr){
        request_answer = {{"request_id"s, stop_id_name.first}, {"buses"s, json::Array()}};
    }else {
        json::Array buses;
        std::set<std::string> b;
        for(const auto& bus : *buses_ptr.value()){
            b.emplace(bus->name);
        }
        for(const auto& bus_name : b){
            buses.emplace_back(bus_name);
        }
        request_answer = {{"request_id"s, stop_id_name.first}, {"buses"s, buses}};
    }
}

void RequestHandler::GetBusStat(const json::Dict& stat_requests, json::Dict& request_answer) const{
    std::pair<int, std::string> bus_id_name = json_reader_ptr->ParseStatRequest(stat_requests);
    std::unordered_map<std::string, double> bus_stat_dict =
            GetBusStat(bus_id_name.second).has_value() ?
                GetBusStat(bus_id_name.second).value() : std::unordered_map<std::string, double>();
    if (!bus_stat_dict.empty()){
        request_answer = {{"request_id"s, bus_id_name.first}
                          , {"curvature"s,      bus_stat_dict.at("curvature"s)}
                          , {"route_length"s,   bus_stat_dict.at("route_length"s)}
                          , {"stop_count"s,     bus_stat_dict.at("stop_count"s)}
                          , {"unique_stop_count"s,  bus_stat_dict.at("unique_stop_count"s)}

        };
    } else {
        request_answer = {{"request_id"s, bus_id_name.first}, {"error_message"s, "not found"s}};
    }
}

svg::Document RequestHandler::RenderMap() const{
    auto buses_ptr = catalogue_ptr->GetBusesPtr();
    return renderer_ptr->RenderMap(buses_ptr);
};

void RequestHandler::GetMapStat(const json::Dict& stat_request, json::Dict& request_answer) const{
    int request_id = stat_request.at("id"s).AsInt();
    svg::Document document = RenderMap();
    std::ostringstream out_strstm;
    document.Render(out_strstm);
    request_answer = {{"request_id"s, request_id}, {"map"s, out_strstm.str() }};
}

} // namespace request_handler
