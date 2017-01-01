cmake_minimum_required(VERSION 3.0)
project(TinyDB)

add_definitions( -DBOOST_ALL_DYN_LINK )

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.47 COMPONENTS thread system filesystem log REQUIRED)
find_package(BISON)
find_package(FLEX)

FLEX_TARGET(SQLLexer ./Parser/SQLLexer.lex ${CMAKE_CURRENT_BINARY_DIR}/Parser.Generated/SQLLexer.cpp)
BISON_TARGET(SQLParser ./Parser/SQLParser.bison ${CMAKE_CURRENT_BINARY_DIR}/Parser.Generated/SQLParser.tab.cpp)

ADD_FLEX_BISON_DEPENDENCY(SQLLexer SQLParser)

include_directories( ${Boost_INCLUDE_DIR} )
include_directories( . )
file(COPY ${FLEX_INCLUDE_DIR}/FlexLexer.h DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Parser.Generated)
link_directories( ${Boost_LIBRARY_DIR} )
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Parser.Generated)

file(GLOB UtilsSources
    "Utils/*.cpp"
)

file(GLOB DatabaseSources
    "Database/*.cpp"
)

file(GLOB StorageSources
    "Storage/*.cpp"
)

file(GLOB ParserSources
    "Parser/*.cpp"
)

file(GLOB FieldTypesSources
    "FieldTypes/*.cpp"
)

file(GLOB IndexSources
    "Index/*.cpp"
)

file(GLOB ExpressionSources
    "Expression/*.cpp"
)

file(GLOB QuerySources
    "Query/*.cpp"
)

file(GLOB ConstraintSources
    "Constraint/*.cpp"
)

add_executable(TinyDB
    main.cpp
    ${UtilsSources}
    ${FieldTypesSources}
    ${DatabaseSources}
    ${StorageSources}
    ${ParserSources}
    ${IndexSources}
    ${ExpressionSources}
    ${QuerySources}
    ${ConstraintSources}
    ${BISON_SQLParser_OUTPUTS}
    ${FLEX_SQLLexer_OUTPUTS}
)

TARGET_LINK_LIBRARIES( TinyDB ${Boost_LIBRARIES} )
