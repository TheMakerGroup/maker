cmake_minimum_required (VERSION 3.15)

if(MINGW OR MSYS OR CYGWIN)
message(FATAL_ERROR "MINGW or MSYS or CYGWIN is not supported")
endif()
project ("maker")

if(WIN32)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "")
endif()

include_directories(./include)
include_directories(depends/yaml-cpp/include)
include_directories(depends/tiny-process-library)

set(YAML_CPP_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp as static library" FORCE)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests" FORCE)

add_subdirectory(depends/yaml-cpp)
add_subdirectory(depends/tiny-process-library)

add_executable (maker  maker.cpp
        "include/print.h" "include/get.h"     "include/execute.hpp"
        "include/get.cpp" "include/print.cpp" "include/execute.cpp"
        "include/shell.hpp"  "include/shell.cpp" "include/root.hpp"
        "include/analyze.hpp" "include/analyze.cpp")

target_compile_definitions(maker PUBLIC YAML_CPP_STATIC_DEFINE)

target_link_libraries(maker PRIVATE yaml-cpp)
target_link_libraries(maker PRIVATE tiny-process-library)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET maker PROPERTY CXX_STANDARD 20)
  set_property(TARGET maker PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
cmake_policy(SET CMP0091 NEW)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
if(UNIX)
set_target_properties(maker PROPERTIES COMPILE_FLAGS "-O2 -Wall -Wextra -pedantic -Werror")
set_target_properties(maker PROPERTIES LINK_FLAGS "-Wl,--gc-sections -Wl,--strip-all")
endif()
endif()
if(WIN32)
string(APPEND CMAKE_CXX_FLAGS " /GR- /W4 /WX -flto")
set(CMAKE_CXXFLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "" FORCE)
string(APPEND CMAKE_EXE_LINKER_FLAGS " /OPT:REF /OPT:ICF /DEBUG:NONE /RELEASE")
endif()
install(TARGETS maker RUNTIME DESTINATION .)