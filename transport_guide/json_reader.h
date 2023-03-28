#pragma once

#include <iostream>
#include <exception>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>

#include "json.h"
#include "map_renderer.h"
#include "domain.h"
#include "geo.h"
#include "svg.h"

#include "transport_catalogue.h"

namespace json_reader{
using namespace json;

class JSONReader{
public:

    JSONReader(transport_catalogue::catalogue::TransportCatalogue& catalogue) : catalogue(catalogue){};

    Dict LoadInput(std::istream& input);

    void FillTransportCatalogue (const Node& base_requests);
    void ParseBaseStopRequest (Dict& stop_request);
    void ParseBaseBusRequest (Dict& bus_request);

    std::pair<int, std::string> ParseStatRequest(const Dict& stat_request) const;

    renderer::MapRendererSettings ParseRenderSettingsRequests(const Dict& rending_setting_request) const;
    void SendOutput(json::Array& stat_data, std::ostream& output) const;
private:
    transport_catalogue::catalogue::TransportCatalogue& catalogue;
};

} //namespace json_reader
