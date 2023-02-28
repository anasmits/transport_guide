#pragma once
#include <string>
#include <iostream>
#include <vector>

#include "transport_catalogue.h"

namespace stat_reader {
using transport_catalogue::catalogue::TransportCatalogue;

void ParseInfoQuery(const TransportCatalogue& catalogue, std::istream& input = std::cin, std::ostream& output = std::cout);
void LoadInfoQuery(const TransportCatalogue& catalogue, std::istream& input = std::cin, std::ostream& output = std::cout);
} // namespace stat_reader
