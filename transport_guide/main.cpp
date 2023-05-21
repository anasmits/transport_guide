#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <cassert>

#include "request_handler.h"
#include "graph.h"
#include "router.h"
#include "transport_router.h"

//#include "test_input_reader.h"
//#include "test_transport_catalogue.h"
//#include "test_stat_reader.h"
//#include "test_json.h"
//#include "test_map_renderer.h"

#include "json_builder.h"

using namespace std;
using namespace transport_catalogue;


//void TestWithFiles(std::string input_file_name, std::string output_file_name){
//    std::ifstream input(input_file_name /*"tsA_case1_input.txt"*/);
//    std::fstream my_output(output_file_name /*"my_output1.txt"*/);
//    if(input.is_open() ){
//        transport_catalogue::catalogue::TransportCatalogue catalogue;
//        input_reader::detail::LoadDataQuery(catalogue, input);
//        if(my_output.is_open()){
//            stat_reader::detail::LoadInfoQuery(catalogue, input, my_output);
//        }
//    }
//    my_output.close();
//    my_output.close();
//}

//void TestAll(){
//    transport_catalogue::test::RunTest();
//    input_reader::test::RunTest();
//    stat_reader::test::RunTest();
//    test_json::RunTest();
////    test_map_renderer::RunTest();


//    transport_catalogue::catalogue::TransportCatalogue catalogue;
////    TestWithFiles("tsC_case1_input.txt", "my_output1.txt");
//}

using namespace graph;



void TestMakeGraph(){
    graph::DirectedWeightedGraph<double> g(7) ;
    vector<size_t> vert{0, 1, 2, 3, 4, 5, 6};
    vector<double> weights{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    vector<graph::Edge<double>> edges {
             {0,1,weights[0]}
            ,{1,2,weights[1]}
            ,{2,3,weights[2]}
            ,{3,1,weights[3]}
            ,{1,4, weights[4]}
            ,{5,6, weights[5]}
    };
    g.AddEdge(edges[0]);
    g.AddEdge(edges[1]);
    g.AddEdge(edges[2]);
    g.AddEdge(edges[3]);
    g.AddEdge(edges[4]);
    g.AddEdge(edges[5]);

    auto answer = graph::Router(g).BuildRoute(0,4);
    auto answer1 = graph::Router(g).BuildRoute(0,0);
    auto answer2 = graph::Router(g).BuildRoute(4,5);
    assert(answer2 == nullopt);
}





void TestCreateCatalogueGraph(std::string input_name, std::string output_name){
    request_handler::RequestHandler rhandler;
    std::ifstream input  (input_name);
    std::ofstream output(output_name);
    if(input.is_open() ){
        rhandler.LoadJson(input).ParseAndProcessQueries(output);
    }
    input.close();
}

int main()
{
//    TestMakeGraph();
//    for(int i = 2; i <=2; ++i) {
//        TestCreateCatalogueGraph("input" + std::to_string(i) + ".json", "output" + std::to_string(i) + ".json");
//    }
    TestCreateCatalogueGraph("in_test_22.json", "output.json");

//    cout << "this is my git project"s << endl;
//    std::istringstream input {
//        "13\n"
//        "Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino\n"
//        "Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino\n"
//        "Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye\n"
//        "Bus 750: Tolstopaltsevo - Marushkino - Marushkino - Rasskazovka\n"
//        "Stop Rasskazovka: 55.632761, 37.333324, 9500m to Marushkino\n"
//        "Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam\n"
//        "Stop Biryusinka: 55.581065, 37.64839, 750m to Universam\n"
//        "Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya\n"
//        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya\n"
//        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye\n"
//        "Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye\n"
//        "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n"
//        "Stop Prazhskaya: 55.611678, 37.603831\n"
//    };
//    std::ostringstream output;
//    transport_catalogue::catalogue::TransportCatalogue catalogue;
//    input_reader::ParseInputQueryPtr(catalogue, input);
//    std::istringstream query{
//        "6\n"
//        "Bus 256\n"
//        "Bus 750\n"
//        "Bus 751\n"
//        "Stop Samara\n"
//        "Stop Prazhskaya\n"
//        "Stop Biryulyovo Zapadnoye\n"
//    };

//    stat_reader::detail::LoadInfoQuery(catalogue, query, output);
//    std::cout << output.str() << endl;

//    TestAll();
//        request_handler::RequestHandler rhandler;
//        std::ifstream input  ("input.json");//("test.json"); //
//        std::ofstream output("output.json");
//        if(input.is_open() ){
//            rhandler.LoadJson(input).ParseAndProcessQueries(output);
//        }
//        input.close();


    return 0;
}
