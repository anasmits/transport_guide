#include "transport_catalogue.h"
#include "stat_reader.h"
#include "input_reader.h"

int main(){
    transport_catalogue::catalogue::TransportCatalogue catalogue;
    input_reader::LoadDataQuery(catalogue, std::cin);
    stat_reader::LoadInfoQuery(catalogue, std::cin, std::cout);
    return 0;
}