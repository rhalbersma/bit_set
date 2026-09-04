#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.28)
project(consumer_add_subdirectory LANGUAGES CXX)

# The repository root, found by walking up rather than through the environment, so this configures outside CI too.
get_filename_component(xstd_bits_root "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)

add_subdirectory("${xstd_bits_root}" xstd-bits-build)

add_executable(main ../main.cpp)
target_link_libraries(main PRIVATE xstd::bits)
