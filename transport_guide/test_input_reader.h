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

namespace transport_catalogue{
namespace input_reader{
namespace test{

using namespace catalogue;

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

void ParseWithDelimeter(){
    using namespace std::literals;
    std::string line = "  55.611087  ,  37.20829 ,  3900m to Marushkino  "s;
    std::vector<std::string> expected = {
      "55.611087"s
    , "37.20829"s
    , "3900m to Marushkino"s};

    auto result = input_reader::ParseWithDelimeter(line, ", "s);
    assert(expected == result);

    std::cerr << "ParseWithDelimeter is OK"s << std::endl;
}

void ParseStopQueryPtr(){
    using namespace std::literals;
    std::string req1 = " Stop  Tolstopaltsevo:   55.611087, 37.20829, 3900m  to   Marushkino "s;
    std::string req2 = "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino, 10m to Everest"s;
    std::string req3 = "Stop R: 55.0001, 37.0001"s;

    TransportCatalogue catalogue;

    input_reader::ParseStopQueryPtr(catalogue, req1);

        assert(catalogue.FindStop("Tolstopaltsevo"s) != nullptr);
        assert(catalogue.FindStop("Tolstopaltsevo"s)->coordinates.lat - 55.611087 < TOL);
        assert(catalogue.FindStop("Tolstopaltsevo"s)->coordinates.lng - 37.20829 < TOL);

        assert(catalogue.FindStop("Marushkino"s) != nullptr);
        assert(catalogue.FindStop("Marushkino"s)->coordinates.lat == 0);
        assert(catalogue.FindStop("Marushkino"s)->coordinates.lng == 0);

        assert(catalogue.GetDistanceBetweenStops(catalogue.FindStop("Tolstopaltsevo"s), catalogue.FindStop("Marushkino"s)) == 3900);

        assert(catalogue.FindStop("Rasskazovka"s) == nullptr);
        assert(catalogue.FindStop("Everest"s) == nullptr);

    input_reader::ParseStopQueryPtr(catalogue, req2);

        assert(catalogue.FindStop("Tolstopaltsevo"s) != nullptr);
        assert(catalogue.FindStop("Marushkino"s) != nullptr);
        assert(catalogue.FindStop("Rasskazovka"s) != nullptr);
        assert(catalogue.FindStop("Everest"s) != nullptr);

        assert(catalogue.FindStop("Marushkino"s)->coordinates.lat - 55.595884 < TOL);
        assert(catalogue.FindStop("Marushkino"s)->coordinates.lng - 37.209755 < TOL);

        assert(catalogue.FindStop("Marushkino"s) != nullptr);
        assert(catalogue.GetDistanceBetweenStops(catalogue.FindStop("Tolstopaltsevo"s), catalogue.FindStop("Marushkino"s)) == 3900);
        assert(catalogue.GetDistanceBetweenStops(catalogue.FindStop("Marushkino"s), catalogue.FindStop("Rasskazovka"s)) == 9900);
        assert(catalogue.GetDistanceBetweenStops(catalogue.FindStop("Marushkino"s), catalogue.FindStop("Marushkino"s)) == 100);
        assert(catalogue.GetDistanceBetweenStops(catalogue.FindStop("Marushkino"s), catalogue.FindStop("Everest"s)) == 10);

        assert(catalogue.FindStop("Rasskazovka"s)->coordinates.lat == 0);
        assert(catalogue.FindStop("Rasskazovka"s)->coordinates.lng == 0);

    input_reader::ParseStopQueryPtr(catalogue, req3);

        assert(catalogue.FindStop("R"s) != nullptr);
        assert(catalogue.FindStop("R"s)->coordinates.lat - 55.0001 < TOL);
        assert(catalogue.FindStop("R"s)->coordinates.lng - 37.0001 < TOL);

    std::cerr << "ParseStopQueryPtr is OK"s << std::endl;
}

void ParseBusQueryPtr(){
    using namespace std::literals;
    std::string req1 = "Stop Biryusinka: 55.0001, 37.0001"s;
    std::string req2 = "Bus  256:   Biryulyovo  Zapadnoye > Biryusinka   > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo  Zapadnoye"s;
    std::string req3 = "Bus 750: Tolstopaltsevo - Marushkino - Marushkino  - Rasskazovka  "s;

    TransportCatalogue catalogue;

    input_reader::ParseStopQueryPtr(catalogue, req1);
    input_reader::ParseBusQueryPtr(catalogue, req2);

        assert(catalogue.FindBus("256"s) != nullptr);
        assert(catalogue.FindBus("256"s)->stops.size() == 6);
        assert(catalogue.FindBus("256"s)->circle_rout == true);
        assert(catalogue.FindStop("Biryulyovo  Zapadnoye"s) != nullptr);
        assert(catalogue.FindStop("Biryulyovo  Zapadnoye"s)->coordinates.lat == 0);
        assert(catalogue.FindStop("Biryulyovo  Zapadnoye"s)->coordinates.lng == 0);
        assert(catalogue.FindStop("Biryusinka"s) != nullptr);
        assert(catalogue.FindStop("Biryusinka"s)->coordinates.lat - 55.0001 < TOL);
        assert(catalogue.FindStop("Biryusinka"s)->coordinates.lng - 37.0001 < TOL);
        assert(catalogue.FindStop("Universam"s) != nullptr);
        assert(catalogue.FindStop("Biryulyovo Tovarnaya"s) != nullptr);
        assert(catalogue.FindStop("Biryulyovo Passazhirskaya"s) != nullptr);
        assert(catalogue.FindStop("Biryulyovo  Zapadnoye"s) != nullptr);

        assert(catalogue.FindBus("750"s) == nullptr);

    input_reader::ParseBusQueryPtr(catalogue, req3);

        assert(catalogue.FindBus("256"s) != nullptr);
        assert(catalogue.FindBus("750"s) != nullptr);
        assert(catalogue.FindBus("750"s)->stops.size() == 7);
        assert(catalogue.FindBus("750"s)->circle_rout == false);
        assert(catalogue.FindStop("Tolstopaltsevo"s) != nullptr);
        assert(catalogue.FindStop("Marushkino"s) != nullptr);
        assert(catalogue.FindStop("Rasskazovka"s) != nullptr);

    std::cerr << "ParseBusQueryPtr is OK"s << std::endl;
}

void ParseInputQueryPtr(){
    using namespace std::literals;
    std::istringstream input {
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
    std::ostringstream output;
    transport_catalogue::catalogue::TransportCatalogue catalogue;
    input_reader::ParseInputQueryPtr(catalogue, input);

    assert(catalogue.FindStop("Tolstopaltsevo"s) != nullptr);
    assert(catalogue.FindStop("Marushkino"s) != nullptr);
    assert(catalogue.FindStop("Rasskazovka"s) != nullptr);
    assert(catalogue.FindStop("Biryulyovo Zapadnoye"s) != nullptr);
    assert(catalogue.FindStop("Biryusinka"s) != nullptr);
    assert(catalogue.FindStop("Universam"s) != nullptr);
    assert(catalogue.FindStop("Biryulyovo Tovarnaya"s) != nullptr);
    assert(catalogue.FindStop("Biryulyovo Passazhirskaya"s) != nullptr);
    assert(catalogue.FindStop("Rossoshanskaya ulitsa"s) != nullptr);
    assert(catalogue.FindStop("Prazhskaya"s) != nullptr);

    assert(catalogue.FindBus("256"s) != nullptr);
    assert(catalogue.FindBus("828"s) != nullptr);
    assert(catalogue.FindBus("750"s) != nullptr);

    std::cerr << "ParseInputQueryPtr is OK"s << std::endl;

}

void RunTest(){
    using namespace std::literals;

    std::cerr << std::endl << "InputReader Test starts" << std::endl;
    ParseInputStream();
    ParseWithDelimeter();
    ParseStopQueryPtr();
    ParseBusQueryPtr();
    ParseInputQueryPtr();


    std::cerr << "Test InputReader is passed"s << std::endl;
}
} // namespace test
} // namespace input_reader
} // namespace transport_catalogue


/*void ParseLine(){
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
*/
