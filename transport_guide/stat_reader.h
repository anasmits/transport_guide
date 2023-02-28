#pragma once

#include <string>
#include <iostream>
#include <vector>

#include "transport_catalogue.h"

namespace stat_reader {
using transport_catalogue::catalogue::TransportCatalogue;

void ParseInfoQuery(const TransportCatalogue& catalogue, std::istream& input = std::cin, std::ostream& output = std::cout);
std::string ParseInfoStopQuery(const TransportCatalogue& catalogue, std::string& line);
std::string ParseInfoBusQuery(const TransportCatalogue& catalogue, std::string& line);
void LoadInfoQuery(const TransportCatalogue& catalogue, std::istream& input = std::cin, std::ostream& output = std::cout);

} // namespace stat_reader
