cmake_minimum_required (VERSION 3.10)


if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
add_compile_options(-O2 -Wall -Wextra -pedantic -Werror)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
add_compile_options(/O2 /W4 /WX)
endif()

project ("maker")

include_directories(./include)
link_directories(lib)

set(YAML_CPP_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp as static library" FORCE)

set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests" FORCE)

add_subdirectory(depends/yaml-cpp)

add_executable (maker  maker.cpp "include/main.h" "include/print.h" "include/get.h" "include/execute.h" 
"include/get.cpp" "include/print.cpp" "include/execute.cpp")

target_compile_definitions(maker PUBLIC YAML_CPP_STATIC_DEFINE)

target_link_libraries(maker PRIVATE yaml-cpp)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET maker PROPERTY CXX_STANDARD 20)
endif()
