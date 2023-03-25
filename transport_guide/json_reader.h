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
#include "domain.h"
#include "geo.h"

#include "transport_catalogue.h"

/*
 * Здесь можно разместить код наполнения транспортного справочника данными из JSON,
 * а также код обработки запросов к базе и формирование массива ответов в формате JSON
 *
 * +1. Получить данные в виде json
 * +2. Прочитать данные из json и сохранить их в двух видах запросов: заполнение базы и
 * запросы к базе на информацию
 * +3. Распарсить данные из заполения базы на остановки и маршруты
 * 4. Распарсить данные из запросов на информации на остановки и маршруты
 * +5. Заполнить базу данных данными (сначала остановками, потом маршрутами)
 * 6. Получить информацию от базы данных по запросам ()
 * 7. Вывести полученную по запросам информацию в заданный поток.
 *
 * Суть такова: парсим в json, отправляем в справочник или получаем из справочника от reader_handler
 */

namespace json_reader{
using namespace json;

void LoadJSON(transport_catalogue::catalogue::TransportCatalogue& catalogue,std::istream& input = std::cin, std::ostream& output = std::cout);
void ParseBaseRequests(transport_catalogue::catalogue::TransportCatalogue& catalogue, const Array& base_requests);
void ParseBaseStopRequest(transport_catalogue::catalogue::TransportCatalogue& catalogue, Dict& stop_request);
void ParseBaseBusRequest(transport_catalogue::catalogue::TransportCatalogue& catalogue, Dict& bus_request);

//void ParseStatRequests(transport_catalogue::catalogue::TransportCatalogue& catalogue, const Array& base_requests, std::ostream& output);
void ParseStatRequests(const request_handler::RequestHandler& rh_, const Array& stat_requests, std::ostream& output);
void ParseStatStopRequest(const request_handler::RequestHandler& rh_, const Dict& stat_request, Dict& request_answer);
void ParseStatBusRequest(const request_handler::RequestHandler& rh_, const Dict& stat_request, Dict& request_answer);




//json::Document LoadJSON(const std::string& s) {
//    std::istringstream strm(s);
//    return json::Load(strm);
//}
} //namespace json_reader
