#pragma once

#include <iostream>
#include <exception>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>

#include "json.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "domain.h"
#include "geo.h"
#include "svg.h"

#include "transport_catalogue.h"

namespace json_reader{
using namespace json;

void LoadJSON(transport_catalogue::catalogue::TransportCatalogue& catalogue, renderer::MapRenderer& renderer, std::istream& input = std::cin, std::ostream& output = std::cout);
void ParseBaseRequests(transport_catalogue::catalogue::TransportCatalogue& catalogue, const Array& base_requests);
void ParseBaseStopRequest(transport_catalogue::catalogue::TransportCatalogue& catalogue, Dict& stop_request);
void ParseBaseBusRequest(transport_catalogue::catalogue::TransportCatalogue& catalogue, Dict& bus_request);

void ParseStatRequests(const request_handler::RequestHandler& rh_, const Array& stat_requests, std::ostream& output);
void ParseStatStopRequest(const request_handler::RequestHandler& rh_, const Dict& stat_request, Dict& request_answer);
void ParseStatBusRequest(const request_handler::RequestHandler& rh_, const Dict& stat_request, Dict& request_answer);
void ParseStatMapRequest(const request_handler::RequestHandler& rh_, const Dict& stat_request, Dict& request_answer);

void ParseRenderSettingsRequests(renderer::MapRenderer& renderer, const Dict& rending_setting_request);


//json::Document LoadJSON(const std::string& s) {
//    std::istringstream strm(s);
//    return json::Load(strm);
//}
} //namespace json_reader
