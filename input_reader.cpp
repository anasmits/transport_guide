#include "input_reader.h"

namespace input_reader{

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
//    result.erase(std::remove(result.begin(), result.end(), ""s), result.end());
    return result;
}

Stop ParseStopQuery(std::string& line){
    using namespace std::literals;
    std::vector<std::string> words = ParseLine(line);
    words.erase(std::remove(words.begin(), words.end(), ""s), words.end()); /* from ParseLine*/
    std::string stop_name = ""s;
    for(size_t i = 1; i < words.size()-2; ++i){
        stop_name += words[i];
        if (stop_name[stop_name.size()-1] == ':'){
            stop_name = stop_name.substr(0, stop_name.size()-1);
            break;
        }
        stop_name += " "s;
    }
    Coordinates coordinates = {std::stod(words[words.size()-2]), std::stod(words[words.size()-1])};
    return {stop_name, coordinates};
}


// works correctly
/*Stop  ParseStopQuery(std::string& line){
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
}*/

// works but with 1 space only between the words of stop || bus name
/*std::tuple<Bus, std::vector<std::string>> ParseBusQuery(std::string& line){
    using namespace std::literals;
    std::vector<std::string> words = ParseLine(line);
    std::string bus_name = ""s;
    size_t i;
    for(i = 1; i < words.size(); ++i){
        bus_name += words[i];
        if(bus_name[bus_name.size()-1] == ':'){
            bus_name = bus_name.substr(0, bus_name.size()-1);
            break;
        }
        bus_name += " "s;
    }
    std::vector<std::string> stops;
    std::string stop;
    bool bus_circular_type = std::find(words.begin(), words.end(), ">"s) != words.end() ? true : false;
    std::string delim = bus_circular_type ? ">"s : "-"s;
    for(++i; i < words.size(); ++i){
        if(words[i] != delim){
            if (!stop.empty()){
                stop += " "s;
            }
            stop += words[i];
        } else {
            stops.push_back(stop);
            stop.clear();
        }
    }
    stops.push_back(stop);
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
}*/

std::tuple<Bus, std::vector<std::string>> ParseBusQuery(std::string& line){
    using namespace std::literals;
    auto name_begin = line.find_first_not_of(' ', line.find_first_of('s')+1);
    auto name_end = line.find_first_of(':');
    auto bus_name = line.substr(name_begin, name_end - name_begin);
    line = line.substr(name_end+1);

    std::vector<std::string> words = ParseLine(line);
    std::vector<std::string> stops;
    std::string stop;
    bool bus_circular_type = std::find(words.begin(), words.end(), ">"s) != words.end() ? true : false;
    std::string delim = bus_circular_type ? ">"s : "-"s;
    for(size_t i = 0; i < words.size(); ++i){
        if(words[i] != delim){
            if (!stop.empty()){
                stop += " "s;
            }
            stop += words[i];
        } else {
            stops.push_back(stop);
            stop.clear();
        }
    }
    stops.push_back(stop);
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

void LoadDataQuery(TransportCatalogue& catalogue, std::istream& input = std::cin){
    ParseInputQuery(catalogue, input);
}

} // namespace input_reader
