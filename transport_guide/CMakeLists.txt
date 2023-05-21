cmake_minimum_required(VERSION 3.5)

project(transport_guide LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(transport_guide main.cpp
                transport_catalogue.h transport_catalogue.cpp
                input_reader.h  input_reader.cpp
                domain.h domain.cpp
                stat_reader.h stat_reader.cpp
                geo.h geo.cpp
                json.h json.cpp
                json_reader.h json_reader.cpp
                json_builder.h json_builder.cpp
                svg.h svg.cpp
                map_renderer.h map_renderer.cpp
                request_handler.h request_handler.cpp
                graph.h
                ranges.h
                router.h


#                test_transport_catalogue.h
#                test_input_reader.h
#                test_stat_reader.h
#                test_json.h
#                test_map_renderer.h
        transport_router.cpp transport_router.h)

install(TARGETS transport_guide
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
