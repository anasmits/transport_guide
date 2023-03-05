#include "input_reader.h"

namespace transport_catalogue{
namespace input_reader{

using namespace transport_catalogue::catalogue;

std::string ReadLine(std::istream& input) {
    std::string s;
    getline(input, s);
    return s;
}

int ReadLineWithNumber(std::istream& input) {
    int result;
    input >> result;
    ReadLine(input);
    return result;
}

std::vector<std::string> ParseWithDelimeter(std::string& line, std::string delim){
    std::vector<std::string> result;
    auto pos = line.find_first_not_of(' ');
    auto pos_npos = line.npos;
    while(pos != pos_npos){
        auto del = line.find(delim, pos);
        result.push_back(del == pos_npos ? line.substr(pos) : line.substr(pos, del-pos));
        pos = line.find_first_not_of(delim, del);
    }
    for(auto& line: result){
        auto begin = line.find_first_not_of(' ');
        auto end = line.find_last_not_of(' ')+1;
        line = line.substr(begin, end - begin);
    }
    return result;
}

// with pointer
void ParseStopQueryPtr(TransportCatalogue& catalogue, std::string& line){
    using namespace catalogue::detail;
    auto name_begin = line.find_first_not_of(' ', line.find_first_of('p')+1);
    auto name_end = line.find_first_of(':', name_begin);
    auto stop_name_from = line.substr(name_begin, name_end - name_begin);

    line = line.substr(name_end+1);
    std::vector<std::string> parsed_line = ParseWithDelimeter(line, ", "s);

    Stop* stop_from = catalogue.FindStop(stop_name_from);
    if (stop_from == nullptr){ //если остановки в каталоге нет, то создаём
        StopPtr stop = StopPtr(stop_name_from);
        stop.Get()->coordinates = {std::stod(parsed_line[0]), std::stod(parsed_line[1])};
        catalogue.AddStop(stop.Get());
    } else { // если есть, то добаляем координаты
        stop_from->coordinates = {std::stod(parsed_line[0]), std::stod(parsed_line[1])};
    }

    if(parsed_line.size() > 2){
        for(size_t i = 2; i < parsed_line.size(); ++i){
            int distance = std::stoi(parsed_line[i].substr(0, parsed_line[i].find_first_of('m')));

            parsed_line[i] = parsed_line[i].substr(parsed_line[i].find_first_of('m')+1);

            auto begin = parsed_line[i].find_first_not_of(' ', parsed_line[i].find_first_of("to"s) +2);
            auto end = parsed_line[i].find_last_not_of(' ')+1;
            std::string stop_name_to = parsed_line[i].substr(begin, end - begin);

            Stop* stop_to= catalogue.FindStop(stop_name_to);
            if(stop_to == nullptr){
                StopPtr stop_empty = StopPtr(stop_name_to);
                catalogue.AddStop(stop_empty.Get());
            }
            catalogue.SetDistanceBetweenStops(catalogue.FindStop(stop_name_from), catalogue.FindStop(stop_name_to), distance);
        }
    }
}

// with pointer
void ParseBusQueryPtr(TransportCatalogue& catalogue, std::string& line){
    using namespace std::literals;
    using namespace catalogue::detail;

    auto name_begin = line.find_first_not_of(' ', line.find_first_of('s')+1);
    auto name_end = line.find_first_of(':');
    auto bus_name = line.substr(name_begin, name_end - name_begin);

    line = line.substr(name_end+1);
    bool bus_circular_type = line.find(">"s) != line.npos ? true : false;
    std::string delim = bus_circular_type ? " > "s : " - "s;

    auto stops = ParseWithDelimeter(line, delim);

    if(bus_circular_type==false){
        size_t i = stops.size();
        stops.resize(stops.size()*2-1);
        for(auto it = next(stops.rbegin(), i); it < stops.rend(); ++it, ++i){
            stops[i] = (*it);
        }
    }

    Bus bus(bus_name, {}, bus_circular_type);
    for(const auto& stop_name : stops){
        Stop* stop = catalogue.FindStop(stop_name);
        if(stop == nullptr){
            StopPtr stop_to_add = StopPtr(stop_name);
            catalogue.AddStop(stop_to_add.Get());
            stop = catalogue.FindStop(stop_name);
        }
        bus.stops.push_back(stop);
    }

    catalogue.AddBus(bus);
}

// function with streaming parsing - if stop doesn't exist, create it with smart pointer
void ParseInputQueryPtr(TransportCatalogue& catalogue, std::istream& input){
    using namespace std::literals;
    int number = ReadLineWithNumber(input);
    for(int i = 0; i < number; ++i){
        std::string line = ReadLine(input);
        if(line[line.find_first_not_of(' ')] == 'S'){
            ParseStopQueryPtr(catalogue, line);
        } else{
            ParseBusQueryPtr(catalogue, line);
        }
    }
}

namespace detail{
void LoadDataQuery(TransportCatalogue& catalogue, std::istream& input = std::cin){
    ParseInputQueryPtr(catalogue, input);
}
} // namespace detail

} // namespace input_reader
} // namespace transport_catalogue


/*
std::vector<std::string> ParseLine(std::string& line){
    using namespace std::literals;
    std::vector<std::string> result;
    int64_t pos = line.find_first_not_of(' ');
    const int64_t pos_end = line.npos;
    while (pos != pos_end){
        int64_t space = line.find(' ', pos);
        result.push_back(space == pos_end ? line.substr(pos) : line.substr(pos, space - pos));
        pos = line.find_first_not_of(' ', space);
    }
    return result;
}

// works correctly
Stop  ParseStopQuery(std::string& line){
    auto name_begin = line.find_first_not_of(' ', line.find_first_of('p')+1);
    auto name_end = line.find_first_of(':', name_begin);
    auto stop_name = line.substr(name_begin, name_end-name_begin);

    auto log_begin = line.find_first_not_of(' ', name_end+1);
    auto log_end = line.find_first_of(' ', log_begin);
    auto log = line.substr(log_begin, log_end - log_begin);

    auto lng_begin = line.find_first_not_of(' ', log_end);
    auto lng_end = line.find_first_of(' ', lng_begin);
    auto lng = line.substr(lng_begin, lng_end - lng_begin);

    Coordinates coordinates = {std::stod(log), std::stod(lng)};
    return {stop_name, coordinates};
}

// works correctly
std::tuple<Bus, std::vector<std::string>> ParseBusQuery(std::string& line){
    using namespace std::literals;
    auto name_begin = line.find_first_not_of(' ', line.find_first_of('s')+1);
    auto name_end = line.find_first_of(':');
    auto bus_name = line.substr(name_begin, name_end - name_begin);
    line = line.substr(name_end+1);

    bool bus_circular_type = line.find(">"s) != line.npos ? true : false;
    std::string delim = bus_circular_type ? " > "s : " - "s;

    auto pos = line.find_first_not_of(' ');
    auto pos_npos = line.npos;
    std::vector<std::string> stops;
    while(pos != pos_npos){
        auto del = line.find(delim, pos);
        stops.push_back(del == pos_npos ? line.substr(pos) : line.substr(pos, del-pos));
        pos = line.find_first_not_of(delim, del);
    }
    for(auto& stop: stops){
        stop = stop.substr(0, stop.find_last_not_of(' ')+1);
    }
    if(bus_circular_type==false){
        std::vector<std::string> add_stops;
        for(auto it = stops.rbegin()+1; it < stops.rend(); ++it){
            add_stops.push_back(*it);
        }
        for(auto stop:add_stops){
            stops.push_back(std::move(stop));
        }
    }
    Bus bus = {bus_name, std::vector<Stop*>(), bus_circular_type};
    return {bus, stops};
}

// works correctly
void ParseInputQuery(TransportCatalogue& catalogue, std::istream& input){
    using namespace std::literals;

    std::vector<std::tuple<Bus, std::vector<std::string>>> bus_queries;
    int number = ReadLineWithNumber(input);
    for(int i = 0; i < number; ++i){
        std::string line = ReadLine(input);
        if(line[line.find_first_not_of(' ')] == 'S'){
            catalogue.AddStop(ParseStopQuery(line));
        } else{
            Bus bus;
            std::vector<std::string> stops;
            std::tie(bus, stops)= ParseBusQuery(line);
            if (CheckIfRouteExists(catalogue, stops)){
                for(auto& stop : stops){
                    bus.stops.push_back(catalogue.FindStop(stop));
                }
                catalogue.AddBus(bus);
            } else{
                bus_queries.push_back({bus, stops});
            }
        }
    }
    if(!bus_queries.empty()){
        for(auto& [bus, stops] : bus_queries){
            if(CheckIfRouteExists(catalogue, stops)){
                for (auto& stop : stops){
                    bus.stops.push_back(catalogue.FindStop(stop));
                }
                catalogue.AddBus(bus);
            }
        }
    }
}

bool CheckIfRouteExists(const TransportCatalogue& catalogue, std::vector<std::string>& stops){
    for(auto& stop : stops){
        if(catalogue.FindStop(stop) == nullptr){
            return false;
        }
    }
    return true;
}

*/
