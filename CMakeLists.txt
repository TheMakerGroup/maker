cmake_minimum_required (VERSION 3.15)


if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
add_compile_options(-O2 -Wall -Wextra -pedantic -Werror)
elseif(MSVC)
add_compile_options(/O2 /W4 /WX)
endif()

project ("maker")

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



if(MSVC)
  set(FLAGS_VARS
    CMAKE_C_FLAGS
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
  )
  foreach(var ${FLAGS_VARS})
    string(REPLACE "/MD" "/MT" ${var} "${${var}}")
    string(REPLACE "/MDd" "/MTd" ${var} "${${var}}")
    string(REPLACE "/W3" "/W4" ${var} "${${var}}")
  endforeach()
endif()