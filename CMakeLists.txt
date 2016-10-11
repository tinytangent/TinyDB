cmake_minimum_required(VERSION 3.0)
project(TinyDB)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.47 COMPONENTS filesystem REQUIRED)

file(GLOB UtilsSources
    "Utils/*.cpp"
)

add_executable(TinyDB
    main.cpp
    ${UtilsSources}
)
