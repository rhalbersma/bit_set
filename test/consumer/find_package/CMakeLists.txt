#          Copyright Rein Halbersma 2014-2026.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.28)
project(consumer_find_package LANGUAGES CXX)

# CMAKE_PREFIX_PATH names the install trees the caller populated. Three of
# them, unlike a library with no dependencies: bit_set's own, xstd's --
# bit_setConfig.cmake find_dependency()s it -- and vcpkg's, for the
# Boost.Hash2 that bit_set's headers hash_append through.
find_package(bit_set 0.1.0 CONFIG REQUIRED)

add_executable(main ../main.cpp)
target_link_libraries(main PRIVATE bit_set::bit_set)
