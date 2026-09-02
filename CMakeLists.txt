#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

cmake_minimum_required(VERSION 3.28)
project(
    bit_set
    VERSION 0.1.0
    DESCRIPTION "A fixed-size ordered set of integers, reimagining std::bitset"
    HOMEPAGE_URL https://github.com/rhalbersma/${PROJECT_NAME}
    LANGUAGES CXX
)

include(GNUInstallDirs)

# Prefer an installed xstd so install(EXPORT) can record it as a dependency; a FetchContent build tree cannot be exported.
find_package(xstd 0.1.0 CONFIG QUIET)
if(NOT xstd_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        xstd
        GIT_REPOSITORY https://github.com/rhalbersma/xstd.git
        # Pinned to a commit on xstd's main, not a branch head; move consumption.yml's dependency_repos with it.
        GIT_TAG 42ab0d409fc9118ea0dc8846508ea62e97b90c7e
    )
    FetchContent_MakeAvailable(xstd)
endif()

# Boost.Hash2 is a public dependency of this library's headers, not just of the tests, so it is declared explicitly.
find_package(boost_hash2 CONFIG REQUIRED)

add_library(
    ${PROJECT_NAME} INTERFACE
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# No target_include_directories: the HEADERS file set supplies both the build and the install interface.
target_sources(
    ${PROJECT_NAME} INTERFACE
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/xstd/bits.hpp
        include/xstd/bits/bit_array.hpp
        include/xstd/bits/bitset.hpp
        include/xstd/bits/bit_finite_set.hpp
        include/xstd/bits/detail/array.hpp
        include/xstd/bits/detail/intrin.hpp
        include/xstd/bits/detail/pred.hpp
        include/xstd/bits/ext/boost.hpp
        include/xstd/bits/ext/boost/dynamic_bitset.hpp
        include/xstd/bits/ext/std.hpp
        include/xstd/bits/ext/std/bitset.hpp
        include/xstd/bits/ext/xstd.hpp
        include/xstd/bits/ext/xstd/bitset.hpp
        include/xstd/bits/ranges.hpp
        include/xstd/bits/ranges/array_view.hpp
        include/xstd/bits/ranges/bit_extent.hpp
        include/xstd/bits/ranges/block_access.hpp
        include/xstd/bits/ranges/set_view.hpp
)

target_link_libraries(
    ${PROJECT_NAME} INTERFACE
    xstd::xstd
    Boost::hash2
)

target_compile_features(
    ${PROJECT_NAME} INTERFACE
    cxx_std_23
)

# Tests and benchmarks exist only when bit_set itself is being developed; consumers get the header-only target.
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(benchmark)
        add_subdirectory(test)
    endif()
endif()

install(
    TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}Targets
    FILE_SET HEADERS
)
install(
    EXPORT ${PROJECT_NAME}Targets
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Package config and version files, ARCH_INDEPENDENT because a header-only library has no architecture-specific binaries.
include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/${PROJECT_NAME}Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    COMPATIBILITY SameMinorVersion
    ARCH_INDEPENDENT
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)
