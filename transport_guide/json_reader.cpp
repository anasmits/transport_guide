#include "json_reader.h"

namespace json_reader{

using namespace std::literals;

Dict JSONReader::LoadInput(std::istream& input){
    Document document = Load(input);
    return document.GetRoot().AsMap();
}

//----------- BaseRequests ----------

void JSONReader::FillTransportCatalogue (const Node& base_requests){
    for(const auto& dict : base_requests.AsArray()){
        auto request = dict.AsMap();
        if (request.at("type"s) == "Stop"s){
            ParseBaseStopRequest(request);
        } else {
            ParseBaseBusRequest(request);
        }
    }
    catalogue.CalculateRouteAndCurvature();
}

void JSONReader::ParseBaseStopRequest(Dict& stop_request){
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

void JSONReader::ParseBaseBusRequest(Dict& bus_request){
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

//----------- StatRequests ----------

std::pair<int, std::string> JSONReader::ParseStatRequest(const Dict& stat_request) const{
    return std::make_pair(stat_request.at("id").AsInt(), stat_request.at("name").AsString());
}

//----------- MapRequests ----------

renderer::MapRendererSettings JSONReader::ParseRenderSettingsRequests(const Dict& rending_setting_request) const{
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
    return settings;
}


} //namespace json_reader




