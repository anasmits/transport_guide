#include <iostream>
#include <fstream>
#include <string>

#include "test_input_reader.h"
#include "test_transport_catalogue.h"
#include "test_stat_reader.h"

//#include "input_reader.h"
//#include "transport_catalogue.h"
//#include "stat_reader.h"

using namespace std;

void TestWithFiles(std::string input_file_name, std::string output_file_name){
    std::ifstream input(input_file_name /*"tsA_case1_input.txt"*/);
    std::fstream my_output(output_file_name /*"my_output1.txt"*/);
    if(input.is_open() ){
        transport_catalogue::catalogue::TransportCatalogue catalogue;
        input_reader::LoadDataQuery(catalogue, input);
        if(my_output.is_open()){
            stat_reader::LoadInfoQuery(catalogue, input, my_output);
        }
    }
    my_output.close();
    my_output.close();
}

void TestAll(){
    transport_catalogue::test::RunTest();
    input_reader::test::RunTest();
    stat_reader::test::RunTest();

    transport_catalogue::catalogue::TransportCatalogue catalogue;
    TestWithFiles("tsB_case1_input.txt", "my_output1.txt");
    TestWithFiles("tsB_case2_input.txt", "my_output2.txt");
}

int main()
{
    cout << "this is my git project"s << endl;
    std::istringstream input {
        "13\n"
        "Stop Tolstopaltsevo: 55.611087, 37.20829\n"
        "Stop Marushkino: 55.595884, 37.209755\n"
        "Bus 256: Biryulyovo  Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo  Zapadnoye\n"
        "Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka\n"
        "Stop Rasskazovka: 55.632761, 37.333324\n"
        "Stop Biryulyovo  Zapadnoye: 55.574371, 37.6517\n"
        "Stop Biryusinka: 55.581065, 37.64839\n"
        "Stop Universam: 55.587655, 37.645687\n"
        "Stop Biryulyovo Tovarnaya: 55.592028, 37.653656\n"
        "Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164\n"
        "Bus 828: Biryulyovo  Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo  Zapadnoye\n"
        "Stop Rossoshanskaya ulitsa: 55.595579, 37.605757\n"
        "Stop Prazhskaya: 55.611678, 37.603831\n"
    };
    std::ostringstream output;
    transport_catalogue::catalogue::TransportCatalogue catalogue;
    input_reader::ParseInputQuery(catalogue, input);
    std::istringstream query{
        "6\n"
        "Bus 256\n"
        "Bus 750\n"
        "Bus 751\n"
        "Stop Samara\n"
        "Stop Prazhskaya\n"
        "Stop Biryulyovo  Zapadnoye\n"
    };
    stat_reader::LoadInfoQuery(catalogue, query, output);
    std::cout << output.str() << endl;

    TestAll();
    return 0;
}
