cmake_minimum_required(VERSION 3.13)
include(FetchContent)


set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
project(freedeck-configurator-companion C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)

FetchContent_Declare(
  cpphttplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(cpphttplib)
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(json)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#add_subdirectory(${cpphttplib_SOURCE_DIR})
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_executable(${PROJECT_NAME} main.cpp)
# set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
# set_target_properties(${PROJECT_NAME} PROPERTIES
#     CXX_STANDARD 20
#     CXX_STANDARD_REQUIRED ON
# )
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC httplib nlohmann_json)
file(COPY "scripts" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# file(COPY "windows.ps1" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})