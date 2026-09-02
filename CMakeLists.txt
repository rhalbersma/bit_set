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

# Prefer an already-installed xstd (so that install(EXPORT ...) below can
# correctly record it as a find_package(xstd) dependency of the installed
# bit_set package); fall back to fetching it for casual local builds where
# xstd isn't installed. FetchContent alone cannot be exported: it produces a
# build-tree target, and install(EXPORT) requires every dependency of an
# exported target to be either part of the same export set or an IMPORTED
# target that itself came from a find_package() call.
find_package(xstd 0.1.0 CONFIG QUIET)
if(NOT xstd_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        xstd
        GIT_REPOSITORY https://github.com/rhalbersma/xstd.git
        # Pinned, not main. This is the only first-party dependency here and it
        # moves under us: aligned_size left <xstd/utility.hpp> for
        # <xstd/memory.hpp>, and every compiling job went red with no commit in
        # this repository. Bump this deliberately, fixing whatever the bump
        # relocates, rather than discovering it from CI.
        #
        # This bump follows xstd's split into xstd/core and xstd/ints, from
        # rhalbersma/xstd#223, #226 and #227: <xstd/memory.hpp> is now
        # <xstd/ints/memory.hpp>, alignment having landed in core and then
        # moved to ints once alignable turned out to be an integer concept.
        # align_up is constrained on alignable rather than on unsigned_integer,
        # which does not reach the call sites here: they pass N as the value and
        # the block width only as the alignment. Name a commit on xstd's main,
        # not a pull request branch head, which merging leaves on no branch.
        # Move .github/workflows/consumption.yml's dependency_repos with this:
        # the find_package consumer builds against an installed xstd from
        # there, not against this fallback, so a stale pin there fails only
        # that one job, with a rename error this configure never sees.
        GIT_TAG 42ab0d409fc9118ea0dc8846508ea62e97b90c7e
    )
    FetchContent_MakeAvailable(xstd)
endif()

# xstd/bits/detail/array.hpp and xstd/bits/bitset.hpp hash_append their state through
# Boost.Hash2 (boost/hash2/{fnv1a,hash_append}.hpp), so it is a real,
# public dependency of this library's headers, not just of the tests -
# declared explicitly here rather than relying on it being pulled in
# incidentally by whatever links Boost::headers.
find_package(boost_hash2 CONFIG REQUIRED)

add_library(
    ${PROJECT_NAME} INTERFACE
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# No target_include_directories: the HEADERS file set below adds its base
# directory to the build interface, and install(TARGETS ... FILE_SET) adds
# the install destination to the install interface.
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

# Tests and benchmarks (and their Boost/fmt/Google Benchmark dependencies)
# only exist when bit_set itself is being developed. Consumers vendoring
# this directory via add_subdirectory or FetchContent get just the
# header-only target, with no BUILD_TESTING workaround required on their
# side.
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

# Package config + version files, so consumers can locate the installed
# package with find_package(bit_set CONFIG). ARCH_INDEPENDENT because a
# header-only library has no architecture-specific binaries.
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
