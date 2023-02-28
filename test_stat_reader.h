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
        "Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length\n"
        "Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length\n"
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
        "Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length\n"
        "Bus 750: 5 stops on route, 3 unique stops, 20939.5 route length\n"
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
        "Stop Tolstopaltsevo: 55.611087, 37.20829\n"
        "Stop Marushkino: 55.595884, 37.209755\n"
        "Bus 256: Biryulyovo  Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
        "Stop Rasskazovka: 55.632761, 37.333324\n"
        "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517\n"
        "Stop Biryusinka: 55.581065, 37.64839\n"
        "Stop Universam: 55.587655, 37.645687\n"
        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
        "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
        "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n"
        "Stop Prazhskaya: 55.611678, 37.603831\n"
    };
    TransportCatalogue catalogue;
    input_reader::ParseInputQuery(catalogue, input);

    std::cerr << std::endl << "Stat Test starts"s << std::endl;

    GetBusInfo(catalogue);
    GetStopInfo(catalogue);
    ParseInfoQuery(catalogue);

    std::cerr << "Test Stat is passed"s << std::endl;
}

} // namespace test
} // namespace stat_reader
