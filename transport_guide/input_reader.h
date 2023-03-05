#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <tuple>
#include <map>

#include "transport_catalogue.h"

namespace transport_catalogue{
namespace input_reader{

using namespace catalogue;

std::string ReadLine(std::istream& input);
int ReadLineWithNumber(std::istream& input);

void ParseStopQueryPtr(TransportCatalogue& catalogue, std::string& line);
void ParseBusQueryPtr(TransportCatalogue& catalogue, std::string& line);
void ParseInputQueryPtr(TransportCatalogue& catalogue, std::istream& input);
std::vector<std::string> ParseWithDelimeter(std::string& line, std::string delim);

namespace detail{
    void LoadDataQuery(TransportCatalogue& catalogue, std::istream& input);
} // namespace detail
} // namespace input_reader
} // namespace transport_catalogue

/*
std::vector<std::string> ParseLine(std::string& line);
Stop ParseStopQuery(std::string& line);
std::tuple<Bus, std::vector<std::string>> ParseBusQuery(std::string& line);
void ParseInputQuery(TransportCatalogue& catalogue, std::istream& input);
bool CheckIfRouteExists(const TransportCatalogue& catalogue, std::vector<std::string>& stops);
*/
