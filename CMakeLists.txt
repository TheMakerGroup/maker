cmake_minimum_required(VERSION 3.15)
project(maker)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded" CACHE STRING "MSVC runtime library type")
endif()

include_directories(./include)
include_directories(depends/yaml-cpp/include)
include_directories(depends/tiny-process-library)

set(YAML_CPP_BUILD_SHARED_LIBS OFF CACHE BOOL "Build yaml-cpp as static library" FORCE)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "Disable yaml-cpp tests" FORCE)

add_subdirectory(depends/yaml-cpp)
add_subdirectory(depends/tiny-process-library)

add_executable(maker
    maker.cpp
    include/print.h
    include/get.h
    include/execute.hpp
    include/get.cpp
    include/print.cpp
    include/execute.cpp
    include/shell.hpp
    include/shell.cpp
    include/root.hpp
    include/analyze.hpp
    include/analyze.cpp
)

set_target_properties(maker PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_definitions(maker PUBLIC YAML_CPP_STATIC_DEFINE)

target_link_libraries(maker PRIVATE yaml-cpp tiny-process-library)

if(MSVC)
    target_compile_options(maker PRIVATE
        /W4
        $<$<BOOL:${WARNINGS_AS_ERRORS}>:/WX>
        /GR-
        $<$<CONFIG:Release>:/GL>
    )
    target_link_options(maker PRIVATE
        $<$<CONFIG:Release>:/LTCG /OPT:REF /OPT:ICF>
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(maker PRIVATE
        -Wall -Wextra -pedantic
        $<$<BOOL:${WARNINGS_AS_ERRORS}>:-Werror>
    )

    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        target_link_options(maker PRIVATE
            $<$<CONFIG:Release>:-Wl,--gc-sections -Wl,--strip-all>
        )
    endif()
endif()

install(TARGETS maker RUNTIME DESTINATION .)