#          Copyright Rein Halbersma 2014-2023.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.17)
project(
    bit_set
    HOMEPAGE_URL https://github.com/rhalbersma/${PROJECT_NAME}
    LANGUAGES CXX
)

add_library(
    ${PROJECT_NAME} INTERFACE
)

set(project_include_dir
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)
target_include_directories(
    ${PROJECT_NAME} INTERFACE
    ${project_include_dir}
)

target_compile_features(
    ${PROJECT_NAME} INTERFACE
    cxx_std_20
)

include(CTest)
add_subdirectory(test)
