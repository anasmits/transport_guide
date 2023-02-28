#pragma once

#include <iostream>
#include <cassert>
#include <vector>

#include "geo.h"
#include "transport_catalogue.h"


namespace transport_catalogue{
namespace test{
using namespace catalogue;

void TransportCatalogueConstructor(){
    using namespace std::literals;
    TransportCatalogue cat;
    std::cerr << "TestConstructor / TestDestructor is OK"s << std::endl;
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

    std::cerr << "TestAddStop is OK"s << std::endl;
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

    cat.AddBus(bus345);

    assert(cat.FindBus("345"s)->name == "345"s);
    assert(cat.FindBus("345"s)->stops == vec_stop2);
    assert(cat.FindBus("345"s)->circle_rout == false);

    cat.AddBus(bus256);
    assert(cat.FindBus("256"s)->name == "256"s);
    assert(cat.FindBus("256"s)->stops == vec_stop1);
    assert(cat.FindBus("256"s)->circle_rout == true);

    std::cerr << "TestAddBus is OK"s << std::endl;
}

void FindStop(){
    using namespace std::literals;
    Stop test_stop1 = {"First"s, {1.1, 2.2}};

    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    assert(cat.FindStop("First"s)->name == "First"s);

    std::cerr << "TestFindStop is OK"s << std::endl;
}

void FindBus(){
    using namespace std::literals;

    Stop test_stop1 = {"First"s, 1.1, 2.2},
                             test_stop2 = {"Second"s, 21.1,22.2},
                             test_stop6 = {"Sixth"s, 61.1,62.2};
    std::vector<Stop*> vec_stop1 = {&test_stop1, &test_stop2};
    std::vector<Stop*> vec_stop2 = {&test_stop1, &test_stop2, &test_stop6, &test_stop2};
    Bus bus256 = {"256"s, vec_stop1, true };
    Bus bus345 = {"345"s, vec_stop2, false };

    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop6);
    cat.AddBus(bus256);
    cat.AddBus(bus345);
    assert(cat.FindBus("256"s)->name == "256"s);
    assert(cat.FindBus("345"s)->name == "345"s);

    std::cerr << "TestFindBus is OK"s << std::endl;
}

void CalculateRouteLength(){
    using namespace std::string_literals;
    Stop test_stop1 = {"First"s, 0, 1.3}
            , test_stop2 = {"Second"s, 1.3 , 2.2}
            , test_stop3 = {"Third"s, 2.5 , 3.7};
    std::vector<Stop*> vec_stop = {&test_stop1, &test_stop2, &test_stop3};
    Bus bus345 = {"345"s, vec_stop, false };

    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop3);
    cat.AddBus(bus345);
    auto expected_route_length = ComputeDistance(test_stop1.coordinates, test_stop2.coordinates)
            + ComputeDistance(test_stop2.coordinates, test_stop3.coordinates);

    assert(cat.CalculateRouteLength(cat.FindBus("345"s)) == expected_route_length);
    std::cerr << "TestCalculateRouteLength is OK"s << std::endl;
}

void GetBusInfo(){
    using namespace std::literals;

    Stop test_stop1 = {"First"s, 0, 0}
                      , test_stop2 = {"Second"s, 0,0}
                      , test_stop6 = {"Sixth"s, 0,0};
    std::vector<Stop*> vec_stop1 = {&test_stop1, &test_stop1};// &test_stop2, &test_stop6, &test_stop1};
    Bus bus("11"s, vec_stop1, true);
    TransportCatalogue cat;
    cat.AddStop(test_stop1);
    cat.AddStop(test_stop2);
    cat.AddStop(test_stop6);
    cat.AddBus(bus);
    std::string expected_bus_info = "Bus 11: 2 stops on route, 1 unique stops, 0 route length"s;
    assert(cat.GetBusInfo("11"s) == expected_bus_info);

    std::cerr << "TestGetBusInfo is OK"s << std::endl;
}

void GetStopInfo(){
    using namespace std::literals;

    Stop test_stop1 = {"First"s, 0, 1}
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

    std::cerr << "TestGetStopInfo is OK"s << std::endl;

}
void RunTest(){
    using namespace std::literals;
    std::cerr << std::endl << "Transport Catalogue Test starts" << std::endl;

    TransportCatalogueConstructor();
    AddStop();
    AddBus();
    FindStop();
    FindBus();
    CalculateRouteLength();
    GetBusInfo();
    GetStopInfo();

    std::cerr << "Test Transport Catalogue is passed"s << std::endl;
}
namespace detail{

}

}
} // namespace transport_catalogue
