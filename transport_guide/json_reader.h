#pragma once

#include <iostream>
#include <exception>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <memory>
#include <algorithm>
#include <string>

#include "json.h"
#include "map_renderer.h"
#include "domain.h"
#include "geo.h"
#include "svg.h"

#include "transport_catalogue.h"
#include "json_builder.h"

namespace json_reader{
using namespace json;

    struct BusRequest {
        std::string name;
        Array stops;
        bool is_roundtrip;
    };

    struct StopRequest {
        std::string name;
        double lat;
        double lng;
        Dict road_distances;
    };

    class Query{
    public:
        virtual ~Query() = 0;
    };
    inline Query::~Query() {}

    class QueryAnswer : public Query {
    public:
        QueryAnswer();
        explicit QueryAnswer(int request_id) : request_id(request_id){}
        virtual json::Dict ToJson() const = 0;
    protected:
        int request_id;
    };

    class NotFound : public QueryAnswer {
    public:
        explicit NotFound(int request_id) : QueryAnswer(request_id){}
            json::Dict ToJson() const override {
                    return json::Builder{}.StartDict()
                                    .Key("request_id").Value(request_id)
                                    .Key("error_message").Value("not found")
                                .EndDict()
                                .Build().AsDict();
            }
    };

    class RouteQuery : public QueryAnswer{
    public:
        RouteQuery() = default;
        RouteQuery(int request_id) : QueryAnswer(request_id){}
        RouteQuery(int request_id, double total_time) : QueryAnswer(request_id), total_time(total_time) {}

        json::Dict ToJson() const override{
            json::Builder result{};

            result.StartDict()
                    .Key("items").StartArray();
                        for(const auto& item : items){
                            result.Value(item->ToJson());
                        }
                        result.EndArray()
                    .Key("request_id").Value(request_id)
                    .Key("total_time").Value(total_time)
                .EndDict()
                .Build().AsDict();

            return result.Build().AsDict();
        };

        void AddItem(RouteQuery* route_query){
            items.push_back(route_query);
        }

        ~RouteQuery(){
            for(auto& item : items){
                delete item;
            }
        }
    private:
        double total_time;
        std::vector<RouteQuery*> items;
    };

    class RouteWait : public RouteQuery{
    public:
        RouteWait() = default;
        explicit RouteWait(int request_id, std::string_view stop_name, double time)
        : RouteQuery(request_id)
        , stop_name(stop_name)
        , time(time)
        {}

        json::Dict ToJson() const override {
            return json::Builder{}.StartDict()
                                    .Key("type").Value("Wait")
                                    .Key("stop_name").Value(std::string(stop_name))
                                    .Key("time").Value(time)
                                .EndDict()
                                .Build().AsDict();
        }
    private:
        std::string_view stop_name;
        double time;
    };

    class RouteMove : public RouteQuery{
    public:
        RouteMove() = default;
        explicit RouteMove(int request_id, std::string_view bus_name, int span_cout, double time)
        : RouteQuery(request_id)
        , bus_name(bus_name)
        , span_count(span_cout)
        , time(time)
        {}

        json::Dict ToJson() const override {
            return json::Builder{}.StartDict()
                                        .Key("type").Value("Bus")
                                        .Key("bus").Value(std::string(bus_name))
                                        .Key("span_count").Value(span_count)
                                        .Key("time").Value(time)
                                    .EndDict()
                                    .Build().AsDict();
        }
    private:
        std::string_view bus_name;
        int span_count;
        double time;
    };

    class Stop : public QueryAnswer{
    public:
        Stop() = default;
        explicit Stop(int request_id, std::set<std::string>& buses) : QueryAnswer(request_id), buses(std::move(buses)){}
        json::Dict ToJson() const override{
            return json::Builder{}.StartDict()
                                    .Key("request_id").Value(request_id)
                                    .Key("buses").Value(MakeArray())
                                .EndDict()
                                .Build().AsDict();
        }
    private:
        json::Array MakeArray() const{
            json::Array result;
            for(const auto& bus_name : buses){
                result.emplace_back(bus_name);
            }
            return result;
        }
        std::set<std::string> buses;
    };

    class Bus : public QueryAnswer{
    public:

        struct BusStat{
            double curvature;
            double route_length;
            double stop_count;
            double unique_stop_count;
        };

        Bus() = default;
        explicit Bus(int request_id, BusStat bus_stat) : QueryAnswer(request_id), bus_stat(bus_stat) {}
        json::Dict ToJson() const override{
           return json::Builder{}.StartDict()
                                   .Key("request_id").Value(request_id)
                                   .Key("curvature").Value(bus_stat.curvature)
                                   .Key("route_length").Value(bus_stat.route_length)
                                   .Key("stop_count").Value(bus_stat.stop_count)
                                   .Key("unique_stop_count").Value(bus_stat.unique_stop_count)
                               .EndDict()
                               .Build().AsDict();
        }
    private:
        BusStat bus_stat;
    };

    class Map : public QueryAnswer{
    public:
        Map() = default;
        explicit Map(int request_id, std::string map_xml) : QueryAnswer(request_id), map_xml(std::move(map_xml)){}
        json::Dict ToJson() const override {
            return json::Builder{}.StartDict()
                                .Key("request_id").Value(request_id)
                                .Key("map").Value(map_xml)
                            .EndDict()
                            .Build().AsDict();
        }
    private:
        std::string map_xml;
    };

    class QueryRequest : public Query {
    public:
        QueryRequest();
        explicit QueryRequest(json::Dict request) : request(request){}
        virtual json::Dict FromJson() const = 0;
    protected:
        json::Dict  request;
    };


class JSONReader{
public:

    JSONReader(transport_catalogue::catalogue::TransportCatalogue& catalogue) : catalogue(catalogue){};

    Dict LoadInput(std::istream& input);

    void FillTransportCatalogue (const Node& base_requests);
    StopRequest ParseBaseStopRequest (const Dict& stop_request) const ;
    void ExecuteBaseStopRequest(const StopRequest&& stop_request);
    BusRequest ParseBaseBusRequest (const Dict& bus_request) const ;
    void ExecuteBaseBusRequest(BusRequest&& bus_request);
    void ParseRoutingSettingsRequest(const Dict& rout_settings);

    void SetRoutingSettings();

    std::pair<int, std::string> ParseStatRequest(const Dict& stat_request) const;

    renderer::MapRendererSettings ParseRenderSettingsRequests(const Dict& rending_setting_request) const;
    void SendOutput(json::Array& stat_data, std::ostream& output) const;
private:
    transport_catalogue::catalogue::TransportCatalogue& catalogue;
};

} //namespace json_reader
