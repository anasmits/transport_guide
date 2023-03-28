#include "json_reader.h"

namespace json_reader{

using namespace std::literals;

void LoadJSON(transport_catalogue::catalogue::TransportCatalogue& catalogue, renderer::MapRenderer& renderer, std::istream& input, std::ostream& output){

    request_handler::RequestHandler rh_(catalogue, renderer);

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

    if (request_dict.find("render_settings"s) == request_dict.end()){
        throw std::invalid_argument("render_settings are invalid"s);
    } else {
        auto render_settings = request_dict.at("render_settings");
        ParseRenderSettingsRequests(renderer, render_settings.AsMap());
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
        } else if (stat_requests[i].AsMap().at("type"s).AsString() == "Bus"s){
            ParseStatBusRequest(rh_, stat_requests[i].AsMap(), request_answer);
        } else {
            ParseStatMapRequest(rh_, stat_requests[i].AsMap(), request_answer);
        }
        stat_answer.emplace_back(request_answer);
    }

    json::Print(Document(stat_answer), output);
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

void ParseStatMapRequest(const request_handler::RequestHandler& rh_, const Dict& stat_request, Dict& request_answer){
    int request_id = stat_request.at("id"s).AsInt();
    svg::Document document = rh_.RenderMap();
    std::ostringstream out_strstm;
    document.Render(out_strstm);
    std::string s = out_strstm.str();
    request_answer = {{"request_id"s, request_id}, {"map"s, s }};
}

void ParseRenderSettingsRequests(renderer::MapRenderer& renderer, const Dict& rending_setting_request){
    using namespace svg;

    double width = rending_setting_request.at("width"s).AsDouble();
    double height = rending_setting_request.at("height"s).AsDouble();

    double padding = rending_setting_request.at("padding"s).AsDouble();

    double line_width = rending_setting_request.at("line_width"s).AsDouble();
    double stop_radius = rending_setting_request.at("stop_radius"s).AsDouble();

    int bus_label_font_size = rending_setting_request.at("bus_label_font_size"s).AsInt();
    Array bus_label_offset_Array = rending_setting_request.at("bus_label_offset"s).AsArray();
    std::vector<double> bus_label_offset = {bus_label_offset_Array[0].AsDouble(), bus_label_offset_Array[1].AsDouble()};

    int stop_label_font_size = rending_setting_request.at("stop_label_font_size"s).AsInt();
    Array stop_label_offset_Array = rending_setting_request.at("stop_label_offset"s).AsArray();
    std::vector<double> stop_label_offset = {stop_label_offset_Array[0].AsDouble(), stop_label_offset_Array[1].AsDouble()};


    double underlayer_width = rending_setting_request.at("underlayer_width"s).AsDouble();

    std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba> underlayer_color = std::monostate();
    if (rending_setting_request.at("underlayer_color"s).IsString()){
        underlayer_color = rending_setting_request.at("underlayer_color"s).AsString();
    } else {
        auto color_array = rending_setting_request.at("underlayer_color"s).AsArray();
        if (color_array.size() == 3){
            underlayer_color = svg::Rgb(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt());
        } else{
            underlayer_color = svg::Rgba(color_array[0].AsInt(), color_array[1].AsInt(), color_array[2].AsInt(), color_array[3].AsDouble());
        }
    }

    Array color_array = rending_setting_request.at("color_palette"s).AsArray();
    std::vector<std::variant<std::monostate, std::string, svg::Rgb, svg::Rgba>> color_palette;

    for(auto& color : color_array){
        if (color.IsString()) {
            color_palette.emplace_back(color.AsString());
        } else {
            auto color_ar = color.AsArray();
            if (color_ar.size() == 3){
                color_palette.emplace_back(svg::Rgb(color_ar[0].AsInt(), color_ar[1].AsInt(), color_ar[2].AsInt()));
            } else {
                color_palette.emplace_back(svg::Rgba(color_ar[0].AsInt(), color_ar[1].AsInt(), color_ar[2].AsInt(), color_ar[3].AsDouble()));
            }
        }
    }

    renderer::MapRendererSettings settings = { width
                                            , height
                                            , padding
                                            , line_width
                                            , stop_radius
                                            , bus_label_font_size
                                            , bus_label_offset
                                            , stop_label_font_size
                                            , stop_label_offset
                                            , underlayer_width
                                            , underlayer_color
                                            , color_palette
                                             };
    renderer.SetSettings(settings);
}


} //namespace json_reader




