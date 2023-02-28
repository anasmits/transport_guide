#include "input_reader.h"
#include "stat_reader.h"

namespace stat_reader{
using transport_catalogue::catalogue::TransportCatalogue;

void ParseInfoQuery(const TransportCatalogue& catalogue, std::istream& input, std::ostream& output){
    using namespace std::literals;
    using namespace input_reader;
    std::string bus_name = ""s;
    int number = ReadLineWithNumber(input);
    for(int i = 0; i < number; ++i){
        std::string line = ReadLine(input);
        auto name_begin = line.find_first_not_of(' ', line.find_first_of('s')+1);
        auto name_end = line.find_first_of(':');
        bus_name = line.substr(name_begin, name_end - name_begin);
        std::string info = catalogue.GetBusInfo(bus_name);
        output << info << std::endl;
        bus_name.clear();
    }
}

void LoadInfoQuery(const TransportCatalogue& catalogue, std::istream& input, std::ostream& output){
    ParseInfoQuery(catalogue, input, output);
}
} // namespace stat_reader