#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <cassert>
#include <windows.h>
#include <sstream>
#include <stdio.h>
# include <cstdlib>
# include <cstdio>

#include "input_reader.h"

namespace input_reader{
namespace test{


void ParseInputStream(){
    using namespace input_reader;
    using namespace std::literals;

    std::istringstream input{
        "2\n"
        "asdfa\n"
        "fdsafdsfs\n"
    };

    std::vector<std::string> lines;
    int number = ReadLineWithNumber(input);
    for(int i = 0; i < number; ++i){
        lines.push_back(ReadLine(input));
    }
    std::vector<std::string> lines_expected = {"asdfa"s, "fdsafdsfs"s};
    assert(lines.size() == number);
    assert(lines[0] == lines_expected[0]);
    assert(lines[1] == lines_expected[1]);
    std::cerr << "Parse input stream is OK"s << std::endl;
}

void ParseLine(){
    using namespace std::literals;

    std::vector<std::string> parsed,
            expected = {"This"s, "is"s, "FINe"s};
    std::string line = " This  is FINe  "s;

    assert(expected == input_reader::ParseLine(line));

    std::cerr << "ParseLine is OK"s << std::endl;
}

void ParseStopQuery(){
    using namespace std::literals;

    transport_catalogue::catalogue::Stop stop_expected = {"Stop1 Stop2"s, {1.3, 1.5}};
    std::string line = " Stop  Stop1 Stop2: 1.3   1.5  "s;

    assert(stop_expected == input_reader::ParseStopQuery(line));

    std::cerr << "ParseStopQuery is OK"s << std::endl;
}

void ParseBusQuery(){
    using namespace std::literals;

    std::string line = "  Bus  21  34: Stop1 >  Stop2 Stop2  "s;

    transport_catalogue::catalogue::Bus bus;
    transport_catalogue::catalogue::Bus bus_expected = {"21  34"s, std::vector<transport_catalogue::catalogue::Stop*>(), true};
    std::vector<std::string> stops, stops_expected = {"Stop1"s, "Stop2 Stop2"s};
    std::tie(bus, stops) = input_reader::ParseBusQuery(line);

    assert(bus == bus_expected);
    assert(stops == stops_expected);

    std::cerr << "ParseBusQuery is OK"s << std::endl;
}

void ParseInputQuery(){
    using namespace std::literals;

    std::istringstream input{
        "4\n"
        "Stop Stop1: 1.1 2.2\n"
        "Bus 23: Stop1 > Stop2\n"
        "Stop Stop2: 1.9 3.2\n"
        "Bus 12: Stop1 - Stop2 - Stop2 - Stop1\n"
    };

    transport_catalogue::catalogue::TransportCatalogue catalogue;
    input_reader::ParseInputQuery(catalogue, input);

    assert(catalogue.FindStop("Stop1"s) != nullptr);
    assert(catalogue.FindStop("Stop2"s) != nullptr);
    assert(catalogue.FindStop("Stop3"s) == nullptr);

    const auto bus1 = catalogue.FindBus("23"s);
    assert(bus1 != nullptr);
    assert(bus1->stops.size() == 2);
    assert(bus1->stops[0]->name == "Stop1"s);
    assert(bus1->stops[1]->name == "Stop2"s);
    assert(bus1->circle_rout == true);

    const auto bus2 = catalogue.FindBus("12"s);
    assert(bus2 != nullptr);
    assert(bus2->stops.size() == 7);
    assert(bus2->stops[0]->name == "Stop1"s);
    assert(bus2->stops[1]->name == "Stop2"s);
    assert(bus2->stops[2]->name == "Stop2"s);
    assert(bus2->stops[3]->name == "Stop1"s);
    assert(bus2->stops[4]->name == "Stop2"s);
    assert(bus2->stops[5]->name == "Stop2"s);
    assert(bus2->stops[6]->name == "Stop1"s);
    assert(bus2->circle_rout == false);

    assert(catalogue.FindBus("1"s) == nullptr);

    std::cerr << "ParseInputQuery is OK"s << std::endl;
}

void RunTest(){
    using namespace std::literals;

    std::cerr << std::endl << "InputReader Test starts" << std::endl;
    ParseInputStream();
    ParseLine();
    ParseStopQuery();
    ParseBusQuery();
    ParseInputQuery();

    std::cerr << "Test InputReader is passed"s << std::endl;
}
}
} // namespace input_reader
