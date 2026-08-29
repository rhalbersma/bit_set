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
        -Wno-lifetime-safety-invalidation                     # ditto; false positive on a.clear() in test/include/set/primitives.hpp treating the container itself as invalidated
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
        -Wno-maybe-uninitialized            # false positive on bit::array's std::array member (WinLibs gcc 15.3.0, Release mode)
    >
)

set(cxx_compile_options_optimization
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -march=native
    >
    # -march=native enables AVX2 on the GitHub Windows runners, and that is what
    # makes the MinGW Debug legs segfault. gdb caught all four crashes on the
    # same instruction:
    #
    #   test.bitset.o1  vmovdqa %ymm0,-0x50(%rbp)   rbp 0xbfcca0 -> 0xbfcc50
    #   test.set.o0     vmovdqa %ymm0,-0xc0(%rbp)   rbp 0xbfd570 -> 0xbfd4b0
    #   test.set.o2     vmovdqa %ymm0,-0xc0(%rbp)   rbp 0xbfd470 -> 0xbfd3b0
    #   test.set.sieve  vmovdqa %ymm0,-0x50(%rbp)   rbp 0xbfcd00 -> 0xbfccb0
    #
    # vmovdqa is a 32-byte *aligned* store. Every one of those targets is inside
    # the mapped stack and every one is 16-byte aligned but not 32-byte aligned -
    # mod 32 == 16, in all four. So this is a general protection fault from an
    # alignment violation, not a bad pointer.
    #
    # The Windows x64 ABI guarantees only 16-byte stack alignment, where the
    # System V ABI's guarantee plus GCC's realignment make the same code safe on
    # Linux. Debug is where it bites because -O0 spills every temporary to a
    # stack slot, so 32-byte ymm stores to the stack are everywhere; -O2 keeps
    # the same values in registers, which is why the Release legs are green.
    #
    # This also settles why nothing separated the four failing tests from their
    # passing neighbours, and why fmt, __uint128_t, std::flat_set and
    # -Wa,-mbig-obj were each ruled out in turn: the crash has nothing to do with
    # what the test does. It lands wherever GCC happened to emit a ymm spill.
    #
    # -mprefer-vector-width=128 caps vectors at 128 bits - exactly the 16 bytes
    # the Windows ABI guarantees - so no 32-byte stack slot is ever needed and no
    # 32-byte aligned store can be emitted. Verified rather than assumed: at -O0
    # with -march=x86-64-v3 the 32-byte copies here use eight ymm registers, and
    # adding this flag brings that to zero.
    #
    # Preferred over -mno-avx, which fixes the crash equally but disables AVX2
    # outright - __AVX2__ stops being defined and -O2 falls back to SSE. This
    # keeps AVX2 and its 128-bit VEX instructions (vpaddq, vpshufb and the rest);
    # only 256-bit vectors go.
    #
    # Scoped to Debug, where the crash lives and where vector width buys nothing
    # anyway. Release keeps the full 256-bit -march=native, which is the only
    # configuration whose measurements depend on it.
    #
    # That scoping rests on an observation rather than a guarantee. Release was
    # green throughout because -O2 keeps these values in registers instead of
    # spilling them - but register pressure can spill a ymm to a 32-byte slot at
    # any optimisation level. If a Release leg ever faults on a vmovdqa, the
    # answer is to widen this to every configuration.
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>,$<CONFIG:Debug>>:
        -mprefer-vector-width=128
    >
)

# Required in every configuration, Debug included. #47 withheld it from Debug
# to test whether -Wa,-mbig-obj combined with -g was what makes the MinGW
# Debug legs segfault. Both halves of that experiment answered at once, and
# the answer is no:
#
#   GCC 15 Debug    assembler: 'file too big' on test/src/set/o0.cpp.obj
#   GCC 17-SVN Debug  same, on set/o0, set/o2 and bitset/o0
#   GCC 16 Debug    assembles, then segfaults in exactly the four tests the
#                   flag was suspected of breaking - test.bitset.o1,
#                   test.set.o0, test.set.o2, test.set.sieve
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

    target_link_libraries(
        header.${header_test_id} PRIVATE
        ${CMAKE_PROJECT_NAME}
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
    string(REPLACE "/" "." target_id ${target_source_dir}/${target_parent_path}/${target_stem})
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
