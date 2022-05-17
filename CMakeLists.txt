cmake_minimum_required(VERSION 3.13)
# set(CMAKE_SYSTEM_NAME Windows)
# set(CMAKE_C_COMPILER /usr/bin/i686-w64-mingw32-gcc)
# set(CMAKE_CXX_COMPILER /usr/bin/i686-w64-mingw32-g++aa)
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

FetchContent_Declare(
  traypp
  GIT_REPOSITORY https://github.com/Soundux/traypp.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(traypp)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
if (WIN32)
	add_executable(${PROJECT_NAME} WIN32 main.cpp)
endif ()
if (UNIX)
	add_executable(${PROJECT_NAME} main.cpp)
endif ()
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC httplib nlohmann_json tray)
file(COPY "icon.ico" "icon.png" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

