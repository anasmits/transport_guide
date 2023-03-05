#pragma once

#include <vector>
#include <sstream>
#include <cassert>

#include "stat_reader.h"
#include "transport_catalogue.h"
#include "input_reader.h"


namespace stat_reader{
namespace test{

void GetStopInfo(transport_catalogue::catalogue::TransportCatalogue& catalogue){
    using namespace std::literals;

    std::istringstream stop_query {
        "Stop Samara\n"
        "Stop Prazhskaya\n"
        "Stop Biryulyovo Zapadnoye\n"
    };

    std::ostringstream output;
    std::string line;
    std::string line_expected =
        "Stop Samara: not found\n"
        "Stop Prazhskaya: no buses\n"
        "Stop Biryulyovo Zapadnoye: buses 256 828\n";

    for(int i = 0; i < 3; ++i){
        getline(stop_query, line);
        output << stat_reader::ParseInfoStopQuery(catalogue, line) << std::endl;
    }

    assert(output.str() == line_expected);

    std::cerr << "ParseInfoStopQuery is OK"s << std::endl;
}

void GetBusInfo(transport_catalogue::catalogue::TransportCatalogue& catalogue){
    using namespace std::literals;   

    std::istringstream bus_query {
        "Bus 256\n"
        "Bus 750\n"
        "Bus 751\n"
    };
    std::ostringstream output;
    std::string line;
    std::string line_expected =
        "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature\n"
        "Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature\n"
        "Bus 751: not found\n";
    for(int i = 0; i < 3; ++i){
        getline(bus_query, line);
        output << stat_reader::ParseInfoBusQuery(catalogue, line) << std::endl;
    }
    std::string answer = output.str();
    assert(answer == line_expected);

    std::cerr << "ParseInfoBusQuery is OK"s << std::endl;
}

void ParseInfoQuery(transport_catalogue::catalogue::TransportCatalogue& catalogue){
    using namespace std::literals;
    using namespace transport_catalogue::catalogue;
    std::istringstream query{
        "6\n"
        "Bus 256\n"
        "Bus 750\n"
        "Bus 751\n"
        "Stop Samara\n"
        "Stop Prazhskaya\n"
        "Stop Biryulyovo Zapadnoye\n"
    };

    std::string line_expected =
        "Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature\n"
        "Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature\n"
        "Bus 751: not found\n"
        "Stop Samara: not found\n"
        "Stop Prazhskaya: no buses\n"
        "Stop Biryulyovo Zapadnoye: buses 256 828\n";
    std::ostringstream output;
    stat_reader::ParseInfoQuery(catalogue, query, output);
    assert(output.str() == line_expected);

    std::cerr << "ParseInfoQuery is OK"s << std::endl;
}

void RunTest(){
    using namespace std::literals;
    using namespace transport_catalogue::catalogue;

    std::istringstream input{
        "13\n"
        "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\n"
        "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\n"
        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
        "Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\n"
        "Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino\n"
        "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"
        "Stop Biryusinka: 55.581065, 37.64839, 750m to Universam\n"
        "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"
        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"
        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"
        "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
        "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n"
        "Stop Prazhskaya: 55.611678, 37.603831\n"
    };
    TransportCatalogue catalogue;
    input_reader::ParseInputQueryPtr(catalogue, input);

    std::cerr << std::endl << "Stat Test starts"s << std::endl;

    GetBusInfo(catalogue);
    GetStopInfo(catalogue);
    ParseInfoQuery(catalogue);

    std::cerr << "Test Stat is passed"s << std::endl;
}

} // namespace test
} // namespace stat_reader
