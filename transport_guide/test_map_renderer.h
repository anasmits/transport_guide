#pragma once

#include "map_renderer.h"

namespace test_map_render{
    void RunTest(){
//        using namespace std;
//        using namespace map_render;

//        const double WIDTH = 600.0;
//        const double HEIGHT = 400.0;
//        const double PADDING = 50.0;

//        // Точки, подлежащие проецированию
//        vector<geo::Coordinates> geo_coords = {
//            {43.587795, 39.716901}, {43.581969, 39.719848}, {43.598701, 39.730623},
//            {43.585586, 39.733879}, {43.590317, 39.746833}
//        };

//        // Создаём проектор сферических координат на карту
//        const map_render::SphereProjector proj{
//            geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
//        };

//        // Проецируем и выводим координаты
//        for (const auto geo_coord: geo_coords) {
//            const svg::Point screen_coord = proj(geo_coord);
//            cout << '(' << geo_coord.lat << ", "sv << geo_coord.lng << ") -> "sv;
//            cout << '(' << screen_coord.x << ", "sv << screen_coord.y << ')' << endl;
//        }
    }
} // namespace test_map_render
