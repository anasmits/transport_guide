#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include <iomanip>

#include "geo.h"
#include "transport_catalogue.h"


namespace transport_catalogue{
namespace test{
using namespace catalogue;

void TransportCatalogueConstructor(){
    using namespace std::literals;
    TransportCatalogue cat;
    std::cerr << "Constructor / Destructor is OK"s << std::endl;
}

void AddStop(){
    using namespace std::literals;
    const double TOL = 0.000001;

    Stop test_stop1 = {"First"s, 1.1, 2.2},
                             test_stop2 = {"Second"s, 1.1, 2.2};

    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    auto stop = cat.FindStop("First"s);
    assert(stop->name == "First"s);
    assert(std::abs(stop->coordinates.lat - 1.1) < TOL);
    assert(std::abs(stop->coordinates.lng - 2.2) < TOL);

    std::cerr << "AddStop is OK"s << std::endl;
}

void AddBus(){
    using namespace std::literals;

    Stop test_stop1 = {"First"s, 1.1, 2.2},
                     test_stop2 = {"Second"s, 21.1,22.2},
                     test_stop3 = {"Third"s, 31.1,32.2},
                     test_stop4 = {"Fourth"s, 41.1,42.2},
                     test_stop5 = {"Fifth"s, 51.1,52.2},
                     test_stop6 = {"Sixth"s, 61.1,62.2};
    std::vector<Stop*> vec_stop1 = {&test_stop1, &test_stop2, &test_stop3};
    std::vector<Stop*> vec_stop2 = {&test_stop4, &test_stop5, &test_stop6};
    Bus bus256 = {"256"s, vec_stop1, true };
    Bus bus345 = {"345"s, vec_stop2, false };

    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop3);
    cat.AddStop(test_stop4);
    cat.AddStop(test_stop5);
    cat.AddStop(test_stop6);

    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 7);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Third"s), 2);
    cat.SetDistanceBetweenStops(cat.FindStop("Fourth"s), cat.FindStop("Fifth"s), 10);
    cat.SetDistanceBetweenStops(cat.FindStop("Sixth"s), cat.FindStop("Fifth"s), 5);

    cat.AddBus(bus256);
    assert(cat.FindBus("256"s)->name == "256"s);
    assert(cat.FindBus("256"s)->stops == vec_stop1);
    assert(cat.FindBus("256"s)->circle_rout == true);

    cat.AddBus(bus345);
    assert(cat.FindBus("345"s)->name == "345"s);
    assert(cat.FindBus("345"s)->stops == vec_stop2);
    assert(cat.FindBus("345"s)->circle_rout == false);

    std::cerr << "AddBus is OK"s << std::endl;
}

void FindStop(){
    using namespace std::literals;
    Stop test_stop1 = {"First"s, {1.1, 2.2}};

    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    assert(cat.FindStop("First"s)->name == "First"s);

    std::cerr << "FindStop is OK"s << std::endl;
}

void FindBus(){
    using namespace std::literals;

    Stop test_stop1 = {"First"s, 1.1, 2.2}
        , test_stop2 = {"Second"s, 21.1,22.2}
        , test_stop6 = {"Sixth"s, 61.1,62.2};
    std::vector<Stop*> vec_stop1 = {&test_stop1, &test_stop2};
    std::vector<Stop*> vec_stop2 = {&test_stop1, &test_stop2, &test_stop6, &test_stop2};
    Bus bus256 = {"256"s, vec_stop1, true };
    Bus bus345 = {"345"s, vec_stop2, false };

    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop6);

    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 7);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Sixth"s), 2);
    cat.SetDistanceBetweenStops(cat.FindStop("Sixth"s), cat.FindStop("Second"s), 10);

    cat.AddBus(bus256);
    cat.AddBus(bus345);
    assert(cat.FindBus("256"s)->name == "256"s);
    assert(cat.FindBus("345"s)->name == "345"s);

    std::cerr << "FindBus is OK"s << std::endl;
}

//// private function
//void CalculateRouteLengthGeo(){
//    using namespace std::string_literals;
//    Stop test_stop1 = {"First"s, 0, 1.3}
//        , test_stop2 = {"Second"s, 1.3 , 2.2}
//        , test_stop3 = {"Third"s, 2.5 , 3.7};
//    std::vector<Stop*> vec_stop = {&test_stop1, &test_stop2, &test_stop3};
//    Bus bus345 = {"345"s, vec_stop, false };

//    TransportCatalogue cat;
//    cat.AddStop(test_stop1);
//    cat.AddStop(test_stop2);
//    cat.AddStop(test_stop3);

//    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 7);
//    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Third"s), 2);

//    cat.AddBus(bus345);
//    auto expected_route_length = ComputeDistance(test_stop1.coordinates, test_stop2.coordinates)
//            + ComputeDistance(test_stop2.coordinates, test_stop3.coordinates);

//    assert(cat.CalculateGeoRouteLength(cat.FindBus("345"s)) == expected_route_length);
//    std::cerr << "CalculateRouteLength is OK"s << std::endl;
//}

//// private function
//void CalculateRouteLength(){
//    using namespace std::string_literals;
//    Stop test_stop1 = {"First"s, 0, 1.3}
//        , test_stop2 = {"Second"s, 1.3 , 2.2}
//        , test_stop3 = {"Third"s, 2.5 , 3.7};
//    std::vector<Stop*> vec_stop = {&test_stop1, &test_stop2, &test_stop3};
//    Bus bus345 = {"345"s, vec_stop, false };
//    TransportCatalogue cat;
//    cat.AddStop(test_stop1);
//    cat.AddStop(test_stop2);
//    cat.AddStop(test_stop3);

//    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 7);
//    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Third"s), 2);

//    cat.AddBus(bus345);

//    assert(cat.CalculateRouteLength(cat.FindBus("345"s)) == 9);
//    std::cerr << "CalculateRouteLength is OK"s << std::endl;
//}

//// private function
//void CalculateCurvature(){
//    using namespace std::string_literals;
//    Stop test_stop1 = {"First"s, 1, 1.3}
//        , test_stop2 = {"Second"s, 1.3 , 2.2};
//    std::vector<Stop*> vec_stop = {&test_stop1, &test_stop2};
//    Bus bus345 = {"345"s, vec_stop, false };
//    TransportCatalogue cat;
//    cat.AddStop(test_stop1);
//    cat.AddStop(test_stop2);

//    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 10);

//    cat.AddBus(bus345);
//    auto geo = ComputeDistance(cat.FindStop("First"s)->coordinates, cat.FindStop("Second"s)->coordinates);

//    assert(cat.CalculateCurvature(geo, cat.GetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s))) - 10/geo < 0.000001);
//    std::cerr << "CalculateCurvature is OK"s << std::endl;
//}

void GetBusInfo(){
    using namespace std::literals;

    Stop test_stop1 = {"First"s, 1, 2}
                      , test_stop2 = {"Second"s, 0.1, 0.2}
                      , test_stop6 = {"Sixth"s, 0,0};
    std::vector<Stop*> vec_stop1 = {&test_stop1, &test_stop2, &test_stop6};// &test_stop2, &test_stop6, &test_stop1};
    Bus bus("11"s, vec_stop1, true);

    auto val = ComputeDistance(test_stop1.coordinates, test_stop2.coordinates) + ComputeDistance(test_stop2.coordinates, test_stop6.coordinates);
    double curvature = 10/val;

    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop6);

    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 8);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Sixth"s), 2);

    cat.AddBus(bus);

    std::ostringstream out;
    out << "Bus 11: 3 stops on route, 3 unique stops, " << std::setprecision(6) << 10 << " route length, "s << std::setprecision(6) << curvature << " curvature"s;
    std::string expected_bus_info = out.str();
    assert(cat.GetBusInfo("11"s) == expected_bus_info);

    assert(cat.GetBusInfo("5"s) == "Bus 5: not found"s);
    std::cerr << "TestGetBusInfo is OK"s << std::endl;
}

void GetStopInfo(){
    using namespace std::literals;

    Stop test_stop1 = {"First"s, 0, 0001}
                      , test_stop2 = {"Second"s, 0,2}
                      , test_stop6 = {"Sixth"s, 0,3}
                      , test_stop8 = {"Eight"s, 0,7};;
    std::vector<Stop*> vec_stop1 = {&test_stop1, &test_stop2};// &test_stop2, &test_stop6, &test_stop1};
    std::vector<Stop*> vec_stop2 = {&test_stop1, &test_stop2, &test_stop6};
    Bus bus1("1"s, vec_stop1, true);
    Bus bus2("2"s, vec_stop2, false);
    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop6);
    cat.AddStop(test_stop8);

    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 10);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("First"s), 100);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Sixth"s), 103);
    cat.SetDistanceBetweenStops(cat.FindStop("Sixth"s), cat.FindStop("Second"s), 9);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("First"s), 3);

    cat.AddBus(bus1);
    cat.AddBus(bus2);

    std::string expected_stop0_info = "Stop Zero: not found"s;
    assert(cat.GetStopInfo("Zero"s) == expected_stop0_info);

    std::string expected_stop8_info = "Stop Eight: no buses"s;
    assert(cat.GetStopInfo("Eight"s) == expected_stop8_info);

    std::string expected_stop1_info = "Stop First: buses 1 2"s;
    assert(cat.GetStopInfo("First"s) == expected_stop1_info);

    std::string expected_stop6_info = "Stop Sixth: buses 2"s;
    assert(cat.GetStopInfo("Sixth"s) == expected_stop6_info);

    std::cerr << "GetStopInfo is OK"s << std::endl;

}

void SetDistanceBetweenStops(){
    using namespace std::literals;
    Stop test_stop1 = {"First"s, 0, 1}
        , test_stop2 = {"Second"s, 0,2}
        , test_stop3 = {"Third"s, 0, 3};
    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop3);

    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 19);
    cat.SetDistanceBetweenStops(cat.FindStop("Third"s), cat.FindStop("Second"s), 31);

    assert(cat.GetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s)) == 19);
    assert(cat.GetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Third"s)) == 31);

    std::cerr << "SetDistanceBetweenStops is OK"s << std::endl;
}

void GetDistanceBetweenStops(){
    using namespace std::literals;
    Stop test_stop1 = {"First"s, 0, 1}
        , test_stop2 = {"Second"s, 0,2}
        , test_stop3 = {"Third"s, 0, 3};
    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 19);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("First"s), 71);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Third"s), 32);

    assert(cat.GetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s)) == 19);
    assert(cat.GetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("First"s)) == 71);
    assert(cat.GetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Third"s)) == 32);
    assert(cat.GetDistanceBetweenStops(cat.FindStop("Third"s), cat.FindStop("Second"s)) == 32);

    std::cerr << "GetDistanceBetweenStops is OK"s << std::endl;
}

void SetBusForStops(){
    using namespace std::literals;
    Stop test_stop1 = {"First"s, 1, 1.3}
        , test_stop2 = {"Second"s, 1.3 , 2.2}
        , test_stop3 = {"Third"s, 1.5 , 2.2};
    std::vector<Stop*> vec_stop = {&test_stop1, &test_stop2};
    std::vector<Stop*> vec_stop1 = {&test_stop1, &test_stop2, &test_stop3};
    Bus bus345 = {"345"s, vec_stop, false };
    Bus bus23 = {"23"s, vec_stop1, false};
    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop3);
    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 10);
    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("First"s), 7);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("Third"s), 18);
    cat.AddBus(bus345);
    cat.AddBus(bus23);

    cat.SetBusForStops(cat.FindBus("345"s)->stops, "345"s);
    cat.SetBusForStops(cat.FindBus("23"s)->stops, "23"s);

    std::set<std::string_view> expected345 = {"345"s, "23"s};
    std::set<std::string_view> expected23 = {"23"s};

    auto result1 = cat.GetBusesForStop(cat.FindStop("First"s));
    auto result2 = cat.GetBusesForStop(cat.FindStop("Second"s));
    auto result3 = cat.GetBusesForStop(cat.FindStop("Third"s));

    assert(result1 == expected345);
    assert(result2  == expected345);
    assert(result3 == expected23);

    std::cerr << "SetBusForStops is OK"s << std::endl;
}

void GetBusesForStop(){
    using namespace std::literals;
    Stop test_stop1 = {"First"s, 1, 1.3}
        , test_stop2 = {"Second"s, 1.3 , 2.2}
        , test_stop3 = {"Third"s, 1.5 , 2.2};
    std::vector<Stop*> vec_stop = {&test_stop1, &test_stop2};
    Bus bus345 = {"345"s, vec_stop, false};
    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop3);
    cat.SetDistanceBetweenStops(cat.FindStop("First"s), cat.FindStop("Second"s), 10);
    cat.SetDistanceBetweenStops(cat.FindStop("Second"s), cat.FindStop("First"s), 100);
    cat.AddBus(bus345);

    cat.SetBusForStops(cat.FindBus("345"s)->stops, "345"s);

    std::set<std::string_view> expected2 = {"345"s};
    std::set<std::string_view> expected_empty = {};

    auto result1 = cat.GetBusesForStop(cat.FindStop("First"s));
    auto result2 = cat.GetBusesForStop(cat.FindStop("Third"s));

    assert(result1 == expected2);
    assert(result2 == expected_empty);

    std::cerr << "GetBusesForStop is OK"s << std::endl;
}

void RunTest(){
    using namespace std::literals;
    std::cerr << std::endl << "Transport Catalogue Test starts" << std::endl;

    TransportCatalogueConstructor();

    AddStop();
    FindStop();

    SetBusForStops();
    GetBusesForStop();

    SetDistanceBetweenStops();
    GetDistanceBetweenStops();

    AddBus();
    FindBus();

    // private functions
//    CalculateRouteLengthGeo();
//    CalculateRouteLength();
//    CalculateCurvature();

    GetStopInfo();
    GetBusInfo();

    std::cerr << "Test Transport Catalogue is passed"s << std::endl;
}
namespace detail{
}

} // namespace test
} // namespace transport_catalogue
