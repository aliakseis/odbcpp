cmake_minimum_required(VERSION 3.6)

project(odbcpp CXX)

FIND_PACKAGE(ODBC)

# specify header and cpp files
file(GLOB odbcpp_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp ${CMAKE_CURRENT_SOURCE_DIR}/include/ODBCLSLB/*.hpp)
file(GLOB odbcpp_cpp ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_library(odbcpp SHARED ${odbcpp_headers} ${odbcpp_cpp})
target_include_directories(odbcpp PRIVATE  ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(odbcpp ${ODBC_LIBRARIES})
