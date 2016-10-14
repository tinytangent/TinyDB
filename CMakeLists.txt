cmake_minimum_required(VERSION 3.0)
project(TinyDB)

add_definitions( -DBOOST_ALL_DYN_LINK )

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.47 COMPONENTS thread system filesystem log REQUIRED)

include_directories( ${Boost_INCLUDE_DIR} )
include_directories( . )
link_directories( ${Boost_LIBRARY_DIR} )

file(GLOB UtilsSources
    "Utils/*.cpp"
)

file(GLOB DatabaseSources
    "Database/*.cpp"
)

file(GLOB StorageSources
    "Storage/*.cpp"
)

add_executable(TinyDB
    main.cpp
    ${UtilsSources}
    ${DatabaseSources}
    ${StorageSources}
)

TARGET_LINK_LIBRARIES( TinyDB ${Boost_LIBRARIES} )
