#include "map_renderer.h"

namespace renderer
{

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }


    //------------- MapRenderer -----------------

    void MapRenderer::SetSettings(MapRendererSettings&& settings){
        settings_ = settings;
    }

    const MapRendererSettings& MapRenderer::GetSettings(){
        return settings_;
    }

    svg::Document MapRenderer::RenderMap(const std::deque<domain::Bus>* buses_ptr) const{
        //Получили вектор указателей на маршруты в лексикографическом порядке по имени
        if ((*buses_ptr).empty()){
            return svg::Document();
        }
        std::vector<const domain::Bus*> buses_sorted_ptr;
        for(const auto& bus : *buses_ptr){
            if (!bus.stops.empty()){
                buses_sorted_ptr.push_back(&bus);
            }
        }
        std::sort(buses_sorted_ptr.begin(), buses_sorted_ptr.end(),
                  [](const auto& lhr, const auto& rhr){return lhr->name < rhr->name; });

        // Записывает вектор координат остановок из непустых маршрутов для SphereProjector, создаём projector
        std::vector<geo::Coordinates> stops_coordinates;
        for(const auto& bus : buses_sorted_ptr){
            for(const auto& stop : bus->stops){
                stops_coordinates.push_back(stop->coordinates);
            }
        }
        SphereProjector projector = SetProjector(stops_coordinates);    

        // Создаём и заполняем документ
        svg::Document document;

        RenderRoutes(document, buses_sorted_ptr, projector);
        RenderRoutesNames(document, buses_sorted_ptr, projector);
        RenderStopsSymbols(document, buses_sorted_ptr, projector);
        RenderStopsNames(document, buses_sorted_ptr, projector);
        return document;
    }

    SphereProjector MapRenderer::SetProjector(const std::vector<geo::Coordinates>& stops_coordinates) const{
        return SphereProjector(stops_coordinates.begin(), stops_coordinates.end(),
                settings_.width, settings_.height, settings_.padding);
    }

    void MapRenderer::RenderRoutes(svg::Document& document,
           const std::vector<const domain::Bus*>& buses_sorted_ptr,
           const SphereProjector& projector) const {

        for(size_t i = 0; i < buses_sorted_ptr.size(); ++i){
            int color_number = i % settings_.color_palette.size();
            auto bus_ptr = buses_sorted_ptr[i];

            svg::Polyline route_line;
            route_line.SetFillColor(svg::NoneColor)
                        .SetStrokeColor(settings_.color_palette[color_number])
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND)
                        .SetStrokeWidth(settings_.line_width);
            for(const auto& stop : bus_ptr->stops){
                route_line.AddPoint(projector(stop->coordinates));
            }
            document.AddPtr(std::make_unique<svg::Polyline>(route_line));
        }
    }

    void MapRenderer::RenderRoutesNames(svg::Document& document,
            const std::vector<const domain::Bus*>& buses_sorted_ptr,
            const SphereProjector& projector) const {

        using namespace std::literals;

        for(size_t i = 0; i < buses_sorted_ptr.size(); ++i){
            int color_number = i % settings_.color_palette.size();
            auto bus_ptr = buses_sorted_ptr[i];
            auto stop1 = bus_ptr->stops[0];
            svg::Text route_name_underlayer;
            route_name_underlayer
                    .SetPosition(projector(stop1->coordinates))
                    .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                    .SetFontSize(settings_.bus_label_font_size)
                    .SetFontFamily("Verdana"s)
                    .SetFontWeight("bold"s)
                    .SetData(bus_ptr->name)
                    .SetFillColor(settings_.underlayer_color)
                    .SetStrokeColor(settings_.underlayer_color)
                    .SetStrokeWidth(settings_.underlayer_width)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            svg::Text route_name;
            route_name
                    .SetPosition(projector(stop1->coordinates))
                    .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                    .SetFontSize(settings_.bus_label_font_size)
                    .SetFontFamily("Verdana"s)
                    .SetFontWeight("bold"s)
                    .SetData(bus_ptr->name)
                    .SetFillColor(settings_.color_palette[color_number]);

            document.Add(route_name_underlayer);
            document.Add(route_name);

            if((bus_ptr->circle_rout == false) && (bus_ptr->stops[(bus_ptr->stops.size())/2] != stop1)){
                auto stop2 = bus_ptr->stops[bus_ptr->stops.size()/2];
                svg::Text route_name_underlayer;
                route_name_underlayer
                        .SetPosition(projector(stop2->coordinates))
                        .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                        .SetFontSize(settings_.bus_label_font_size)
                        .SetFontFamily("Verdana"s)
                        .SetFontWeight("bold"s)
                        .SetData(bus_ptr->name)
                        .SetFillColor(settings_.underlayer_color)
                        .SetStrokeColor(settings_.underlayer_color)
                        .SetStrokeWidth(settings_.underlayer_width)
                        .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                svg::Text route_name;
                route_name
                        .SetPosition(projector(stop2->coordinates))
                        .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
                        .SetFontSize(settings_.bus_label_font_size)
                        .SetFontFamily("Verdana"s)
                        .SetFontWeight("bold"s)
                        .SetData(bus_ptr->name)
                        .SetFillColor(settings_.color_palette[color_number]);

                document.Add(route_name_underlayer);
                document.Add(route_name);
            }
        }
    }

    void MapRenderer::RenderStopsSymbols(svg::Document& document,
            const std::vector<const domain::Bus*>& buses_sorted_ptr,
            const SphereProjector& projector) const {

        using namespace std::literals;

        // Получаем отсортированный по имени остановки сет от всех остановок маршрутов
        using StopPtr = domain::Stop*;
        struct cmp{
            bool operator()(const StopPtr& lh, const StopPtr& rh) const { return lh->name < rh->name;} ;
        };

        std::set<StopPtr, cmp> stops_sorted_ptr;
        for(const auto& bus_ptr : buses_sorted_ptr){
            for(const auto& stop : bus_ptr->stops){
                stops_sorted_ptr.insert(stop);
            }
        }

        for(const auto& stop : stops_sorted_ptr){
            document.Add(svg::Circle()
                            .SetCenter(projector(stop->coordinates))
                            .SetRadius(settings_.stop_radius)
                            .SetFillColor("white"s)
            );
        }
    }

    void MapRenderer::RenderStopsNames(svg::Document& document,
            const std::vector<const domain::Bus*>& buses_sorted_ptr,
            const SphereProjector& projector) const{

        using namespace std::literals;

        // Получаем отсортированный по имени остановки сет от всех остановок маршрутов
        using StopPtr = domain::Stop*;
        struct cmp{
            bool operator()(const StopPtr& lh, const StopPtr& rh) const { return lh->name < rh->name;} ;
        };

        std::set<StopPtr, cmp> stops_sorted_ptr;
        for(const auto& bus_ptr : buses_sorted_ptr){
            for(const auto& stop : bus_ptr->stops){
                stops_sorted_ptr.insert(stop);
            }
        }

        for(const auto& stop : stops_sorted_ptr){
            svg::Text stop_name_underlayer;
            stop_name_underlayer
                    .SetPosition(projector(stop->coordinates))
                    .SetOffset({settings_.stop_label_offset[0], settings_.stop_label_offset[1]})
                    .SetFontSize(settings_.stop_label_font_size)
                    .SetFontFamily("Verdana"s)
                    .SetData(stop->name)

                    .SetFillColor(settings_.underlayer_color)
                    .SetStrokeColor(settings_.underlayer_color)
                    .SetStrokeWidth(settings_.underlayer_width)
                    .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

            svg::Text stop_name;
            stop_name
                    .SetPosition(projector(stop->coordinates))
                    .SetOffset({settings_.stop_label_offset[0], settings_.stop_label_offset[1]})
                    .SetFontSize(settings_.stop_label_font_size)
                    .SetFontFamily("Verdana"s)
                    .SetData(stop->name)
                    .SetFillColor("black"s);

            document.Add(stop_name_underlayer);
            document.Add(stop_name);
        }

    }

} // namespace renderer
