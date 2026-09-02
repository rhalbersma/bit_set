#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

set(Boost_USE_STATIC_LIBS ON)
find_package(
    Boost CONFIG REQUIRED COMPONENTS
    unit_test_framework
)
find_package(fmt REQUIRED)
find_package(range-v3 CONFIG REQUIRED)

# clang-cl reports CXX_COMPILER_ID Clang but only understands its MSVC-style
# driver flags, not GNU-style ones like -pedantic-errors.
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(bit_set_is_clang_cl ON)
else()
    set(bit_set_is_clang_cl OFF)
endif()

set(cxx_compile_definitions
    BOOST_ALL_NO_LIB
    BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
    BOOST_TEST_MAIN
    $<$<CXX_COMPILER_ID:MSVC>:
        _SCL_SECURE_NO_WARNINGS
    >
)

set(cxx_compile_options_warnings
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
        /permissive-
        /WX
        /bigobj
    >
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Werror
    >
    $<$<AND:$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>,$<NOT:$<BOOL:${bit_set_is_clang_cl}>>>:
        -pedantic-errors
    >
    $<$<CXX_COMPILER_ID:Clang>:
        -Weverything
        -Wno-unknown-warning-option         # forward compatibility for new Clang warnings
        -Wno-lifetime-safety-intra-tu-suggestions             # Clang trunk (23-SVN); suggests [[clang::lifetimebound]] across xstd::proxy::bidirectional and xstd::bitset - not yet acted on
        -Wno-lifetime-safety-intra-tu-constructor-suggestions # ditto, for constructors
        -Wno-lifetime-safety-invalidation                     # ditto; false positive on a.clear() in test/include/test/set/primitives.hpp treating the container itself as invalidated
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-c++20-compat-pedantic
        -Wno-nrvo
        -Wno-unsafe-buffer-usage
        -Wno-disabled-macro-expansion       # triggered by Boost.Test
        -Wno-global-constructors            # triggered by Boost.Test
        -Wno-used-but-marked-unused         # triggered by Boost.Test
        -Wno-c2y-extensions                 # triggered by Boost.Test's use of __COUNTER__
        -Wno-ctad-maybe-unsupported         # std::reverse_iterator(it) uses the standard's own deduction guide
        -Wno-zero-as-null-pointer-constant  # triggered by <=> comparisons against 0
    >
    $<$<CXX_COMPILER_ID:GNU>:
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion                        # NOTE: possibly triggered by range-v3 views::zip overload resolution
        -Wnrvo
        -Wshadow
        -Wsign-compare
        -Wsign-conversion
        -Wsign-promo
    >
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:Release>>:
        -Wno-array-bounds                   # triggered by Boost.DynamicBitset for gcc >= 12 in Release mode
        -Wno-stringop-overflow              # triggered by Boost.DynamicBitset for gcc >= 12 in Release mode
        -Wno-maybe-uninitialized            # false positive on detail::array's std::array member (WinLibs gcc 15.3.0, Release mode)
    >
)

set(cxx_compile_options_optimization
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -march=native
    >
    # MinGW emits 32-byte aligned stores to a 16-byte aligned stack; the assembler rewrites
    # them unaligned, keeping 256-bit vectors (#54, GCC PR target/54412, binutils 2.38+).
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:
        -Wa$<COMMA>-muse-unaligned-vector-move
    >
)

# Required in every configuration, Debug included. #47 withheld it from Debug
# to test whether -Wa,-mbig-obj combined with -g was what makes the MinGW
# Debug legs segfault. Both halves of that experiment answered at once, and
# the answer is no:
#
#   GCC 15 Debug    assembler: 'file too big' on test/src/bits/std_set/o0.cpp.obj
#   GCC 17-SVN Debug  same, on std_set/o0, std_set/o2 and
#                     std_bitset/o0
#   GCC 16 Debug    assembles, then segfaults in exactly the four tests the
#                   flag was suspected of breaking - test.bits.std_bitset.o1,
#                   test.bits.std_set.o0, test.bits.std_set.o2,
#                   test.bits.std_set.sieve
#
# Two rungs out of three cannot even assemble these objects without the flag,
# so it is load-bearing. The third assembles without it and crashes anyway,
# so the flag is not what causes the crash. -g1 and split DWARF were the
# remedies for the interaction hypothesis; there is no interaction to remedy.
# The MinGW Debug segfaults remain open and are unrelated to this line.
set(cxx_compile_options_mbig_obj
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:
	    -Wa,-mbig-obj
    >
)

# Each public header is compiled as its own translation unit (linked against
# the same warning flags as the unit tests), proving it is self-sufficient.
# This is what the Clang-Tidy workflow analyzes - the Boost.Test sources
# below are left out of that analysis on purpose: the test macros drown the
# signal in noise.
set(public_header_dir ${PROJECT_SOURCE_DIR}/include)
file(
    GLOB_RECURSE public_headers
    CONFIGURE_DEPENDS
    RELATIVE ${public_header_dir}
    ${public_header_dir}/xstd/*.hpp
)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency)

foreach(h ${public_headers})
    string(REPLACE "/" "." header_test_id ${h})
    string(REGEX REPLACE "[.]hpp$" "" header_test_id ${header_test_id})
    set(header_test_source ${CMAKE_CURRENT_BINARY_DIR}/header_self_sufficiency/${header_test_id}.cpp)
    file(
        WRITE ${header_test_source}
        "#include <${h}>\nint main() {}\n"
    )

    add_executable(header.${header_test_id} ${header_test_source})

    # Boost::headers because xstd/bits/ext/boost/dynamic_bitset.hpp adapts a
    # header-only Boost library, and the self-sufficiency units under ext/ have
    # to see it. The bit_set target itself carries only Boost::hash2.
    target_link_libraries(
        header.${header_test_id} PRIVATE
        ${CMAKE_PROJECT_NAME}
        Boost::headers
    )

    target_compile_definitions(
        header.${header_test_id} PRIVATE
        ${cxx_compile_definitions}
    )

    target_compile_options(
        header.${header_test_id} PRIVATE
        ${cxx_compile_options_warnings}
        ${cxx_compile_options_optimization}
        ${cxx_compile_options_mbig_obj}
    )

    add_test(NAME header.${header_test_id} COMMAND header.${header_test_id})
endforeach()

# The test tree mirrors the header tree: every public header is answered by
# exactly one test source at the same path, so a reviewer reading
# include/xstd/bits/ranges/set_view.hpp knows where its tests are without
# looking. Enforced here so it cannot drift.
#
# detail/ is excluded. It is machinery rather than interface -- the storage
# vehicles the containers are built from -- and it is tested through the
# containers that carry it, not directly. It still gets a header
# self-sufficiency target above; what it does not get is a mirrored source.
file(
    GLOB_RECURSE mirrored_headers
    CONFIGURE_DEPENDS
    RELATIVE ${public_header_dir}
    ${public_header_dir}/xstd/*.hpp
)
list(FILTER mirrored_headers EXCLUDE REGEX "/detail/")

set(unmirrored_headers)
foreach(h ${mirrored_headers})
    # xstd/bits/ranges/set_view.hpp is answered by test/src/bits/ranges/set_view.cpp.
    string(REGEX REPLACE "^xstd/" "" mirrored ${h})
    string(REGEX REPLACE "[.]hpp$" ".cpp" mirrored ${mirrored})
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${mirrored})
        list(APPEND unmirrored_headers "    include/${h}\n      -> expected test/src/${mirrored}")
    endif()
endforeach()

if(unmirrored_headers)
    list(JOIN unmirrored_headers "\n" unmirrored_report)
    message(
        FATAL_ERROR
        "These headers have no mirrored test source:\n${unmirrored_report}\n"
        "Every public header is mirrored by exactly one test source; add it, "
        "or the mirror is no longer a rule."
    )
endif()

set(current_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(current_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
# The pattern is anchored to current_src_dir. RELATIVE only controls how
# the results are reported, not where the search runs: an unanchored
# *.cpp resolves against CMAKE_CURRENT_SOURCE_DIR, i.e. test/ rather than
# test/src/, and so swept up every .cpp anywhere under test/ -- including
# test/consumer/main.cpp, which then compiled with BOOST_TEST_MAIN and
# collided with the main Boost.Test generates.
file(GLOB_RECURSE targets RELATIVE ${current_src_dir} ${current_src_dir}/*.cpp)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME target_source_dir)
foreach(t ${targets})
    cmake_path(GET t PARENT_PATH target_parent_path)
    cmake_path(GET t STEM        target_stem       )
    # A source directly under src/ has an empty parent path, which used to
    # leave an empty component in the middle -- test/src/bits.cpp came out as
    # test..bits. Collapse runs of separators before splitting on them, so the
    # id is the path and nothing else.
    string(REPLACE "/" "." target_id ${target_source_dir}/${target_parent_path}/${target_stem})
    string(REGEX REPLACE "[.]+" "." target_id ${target_id})
    string(REGEX REPLACE "^[.]" "" target_id ${target_id})

    add_executable(${target_id} src/${t})

    target_link_libraries(
        ${target_id} PRIVATE
        ${CMAKE_PROJECT_NAME}
        Boost::headers
        Boost::unit_test_framework
        fmt::fmt
        range-v3::range-v3
    )

    target_include_directories(
        ${target_id} PRIVATE
        ${current_include_dir}
    )

    target_compile_definitions(
        ${target_id} PRIVATE
        ${cxx_compile_definitions}
    )

    target_compile_options(
        ${target_id} PRIVATE
        ${cxx_compile_options_warnings}
        ${cxx_compile_options_optimization}
        ${cxx_compile_options_mbig_obj}
    )

    add_test(${target_id} ${target_id})
endforeach()
