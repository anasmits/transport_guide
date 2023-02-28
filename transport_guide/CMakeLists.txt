cmake_minimum_required(VERSION 3.5)

project(transport_guide LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(transport_guide main.cpp
                transport_catalogue.h
                transport_catalogue.cpp
                input_reader.h
                input_reader.cpp
                stat_reader.h
                stat_reader.cpp
                geo.h

                test_transport_catalogue.h
                test_input_reader.h
                test_stat_reader.h
            )

install(TARGETS transport_guide
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
