#include "stat_reader.h"
#include "input_reader.h"

namespace stat_reader{
using transport_catalogue::catalogue::TransportCatalogue;

void ParseInfoQuery(const TransportCatalogue& catalogue, std::istream& input, std::ostream& output){
    using namespace std::literals;
    using namespace input_reader;
    std::string bus_name = ""s;
    int number = ReadLineWithNumber(input);
    for(int i = 0; i < number; ++i){
        std::string line = ReadLine(input);
        std::string info = ""s;
        if(line[line.find_first_not_of(' ')] == 'B'){
            info = ParseInfoBusQuery(catalogue, line);
        }else{
            info = ParseInfoStopQuery(catalogue, line);
        }
        output << info << std::endl;
    }
}

std::string ParseInfoStopQuery(const TransportCatalogue& catalogue, std::string& line){
    auto name_begin = line.find_first_not_of(' ', line.find_first_of('p')+1);
    auto name_end = line.find_last_not_of(' ')+1;
    auto stop_name = line.substr(name_begin, name_end - name_begin);
    return catalogue.GetStopInfo(stop_name);
}

std::string ParseInfoBusQuery(const TransportCatalogue& catalogue, std::string& line){
    auto name_begin = line.find_first_not_of(' ', line.find_first_of('s')+1);
    auto name_end = line.find_last_not_of(' ')+1;
    auto bus_name = line.substr(name_begin, name_end - name_begin);
    return catalogue.GetBusInfo(bus_name);
}

void LoadInfoQuery(const TransportCatalogue& catalogue, std::istream& input, std::ostream& output){
    ParseInfoQuery(catalogue, input, output);
}
} // namespace stat_reader
