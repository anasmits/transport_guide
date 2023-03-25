#include "json_reader.h"

namespace json_reader{

using namespace std::literals;

void LoadJSON(transport_catalogue::catalogue::TransportCatalogue& catalogue, std::istream& input, std::ostream& output){

    request_handler::RequestHandler rh_(catalogue);

    using namespace json;

    Document document = Load(input);
    Dict request_dict = document.GetRoot().AsMap();
    if (request_dict.empty()){
        throw std::out_of_range("Request is empty"s);
    }
    if (request_dict.find("base_requests"s) == request_dict.end()){
        throw std::invalid_argument("base_requests are invalid"s);
    } else {
        auto base_requests = request_dict.at("base_requests");
        ParseBaseRequests(catalogue, base_requests.AsArray());
    }
    if (request_dict.find("stat_requests"s) == request_dict.end()){
        throw std::invalid_argument("stat_requests are invalid"s);
    } else {
        auto stat_requests = request_dict.at("stat_requests");
        ParseStatRequests(rh_, stat_requests.AsArray(), output);
    }
}

void ParseBaseRequests(transport_catalogue::catalogue::TransportCatalogue& catalogue, const Array& base_requests){
    for(const auto& dict : base_requests){
        auto request = dict.AsMap();
        if (request.at("type"s) == "Stop"s){
            ParseBaseStopRequest(catalogue, request);
        } else {
            ParseBaseBusRequest(catalogue, request);
        }
    }
    catalogue.CalculateRouteAndCurvature();
}

void ParseBaseStopRequest(transport_catalogue::catalogue::TransportCatalogue& catalogue, Dict& stop_request){
    std::string stop_name_from = stop_request.at("name"s).AsString();
    double latitude = stop_request.at("latitude"s).AsDouble();
    double longitude = stop_request.at("longitude"s).AsDouble();
    Dict road_distances = stop_request.at("road_distances"s).AsMap();

    domain::Stop* stop_from = catalogue.FindStop(stop_name_from);
    if (stop_from == nullptr){
        auto stop = std::make_unique<domain::Stop>(stop_name_from, latitude, longitude);
        catalogue.AddStop(stop.get());
    } else {
        stop_from->coordinates = {latitude, longitude};
    }

    if (!road_distances.empty()){
        for(const auto& [stop_name_to, distance] : road_distances){
            if(catalogue.FindStop(stop_name_to) == nullptr){
                geo::Coordinates coordinates = { .0, .0};
                auto stop = std::make_unique<domain::Stop>(stop_name_to, coordinates);
                catalogue.AddStop(stop.get());
            }
            catalogue.SetDistanceBetweenStops(catalogue.FindStop(stop_name_from), catalogue.FindStop(stop_name_to), distance.AsInt());
        }
    }
}

void ParseBaseBusRequest(transport_catalogue::catalogue::TransportCatalogue& catalogue, Dict& bus_request){
    std::string bus_name = bus_request.at("name"s).AsString();
    Array stops = bus_request.at("stops"s).AsArray();
    bool is_roundtrip = bus_request.at("is_roundtrip"s).AsBool();

    if (is_roundtrip == false){
        size_t i = stops.size();
        stops.resize(stops.size()*2-1);
        for(auto it = next(stops.rbegin(), i); it < stops.rend(); ++it, ++i){
            stops[i] = (*it);
        }
    }

    domain::Bus bus(bus_name, {}, is_roundtrip);
    for(const auto& stop_name : stops){
        auto stop = catalogue.FindStop(stop_name.AsString());
        if (stop == nullptr){
            geo::Coordinates coordinates = { .0, .0};
            auto stop_to_add = std::make_unique<domain::Stop>(stop_name.AsString(), coordinates);
            catalogue.AddStop(stop_to_add.get());
            stop = catalogue.FindStop(stop_name.AsString());
        }
        bus.stops.push_back(stop);
    }

    catalogue.AddBus(bus);
}

void ParseStatRequests(const request_handler::RequestHandler& rh_, const Array& stat_requests, std::ostream& output){
    Array stat_answer;

    for (size_t i = 0; i < stat_requests.size(); ++i) {
        if (stat_requests[i].AsMap().empty()){
            continue;
        }
        Dict request_answer;
        if (stat_requests[i].AsMap().at("type"s).AsString() == "Stop"s){
            ParseStatStopRequest(rh_, stat_requests[i].AsMap(), request_answer );
        } else {
            ParseStatBusRequest(rh_, stat_requests[i].AsMap(), request_answer);
        }
        stat_answer.emplace_back(request_answer);
    }

    json::Print(Document(stat_answer),output);
}

void ParseStatStopRequest(const request_handler::RequestHandler& rh_, const Dict& stat_request, Dict& request_answer){
    std::string name = stat_request.at("name"s).AsString();
    int request_id = stat_request.at("id"s).AsInt();

    const auto buses_ptr = rh_.GetBusesByStop(name);
    if (buses_ptr == std::nullopt){
        request_answer = {{"request_id"s, request_id}, {"error_message"s, "not found"}};
    } else if (buses_ptr.value() == nullptr){
        request_answer = {{"request_id"s, request_id}, {"buses"s, Array()}};
    }else {
        Array buses;
        std::set<std::string> b;
        for(const auto& bus : *buses_ptr.value()){
//            buses.emplace_back(bus->name);
            b.emplace(bus->name);
        }
        for(const auto& bus_name : b){
            buses.emplace_back(bus_name);
        }

        request_answer = {{"request_id"s, request_id}, {"buses"s, buses}};
    }
}

void ParseStatBusRequest(const request_handler::RequestHandler& rh_, const Dict& stat_request, Dict& request_answer){
    std::string name = stat_request.at("name"s).AsString();
    int request_id = stat_request.at("id"s).AsInt();   

    std::unordered_map<std::string, double> bus_stat_dict = rh_.GetBusStat(name).has_value() ? rh_.GetBusStat(name).value() : std::unordered_map<std::string, double>();
    if (!bus_stat_dict.empty()){
        request_answer = {{"request_id"s, request_id}
                          , {"curvature"s,      bus_stat_dict.at("curvature"s)}
                          , {"route_length"s,   bus_stat_dict.at("route_length"s)}
                          , {"stop_count"s,     bus_stat_dict.at("stop_count"s)}
                          , {"unique_stop_count"s,  bus_stat_dict.at("unique_stop_count"s)}

        };
    } else {
        request_answer = {{"request_id"s, request_id}, {"error_message"s, "not found"s}};
    }
}

} //namespace json_reader




