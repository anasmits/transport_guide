#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <tuple>
#include <map>

#include "transport_catalogue.h"

namespace input_reader{

std::string ReadLine(std::istream& input);
int ReadLineWithNumber(std::istream& input);

using namespace transport_catalogue::catalogue;

std::vector<std::string> ParseLine(std::string& line);
Stop ParseStopQuery(std::string& line);
std::tuple<Bus, std::vector<std::string>> ParseBusQuery(std::string& line);
void ParseInputQuery(TransportCatalogue& catalogue, std::istream& input);
bool CheckIfRouteExists(const TransportCatalogue& catalogue, std::vector<std::string>& stops);

void LoadDataQuery(TransportCatalogue& catalogue, std::istream& input);

} // namespace input_reader
