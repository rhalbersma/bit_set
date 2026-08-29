#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost CONFIG REQUIRED)
find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)

# clang-cl reports CXX_COMPILER_ID Clang but only understands its MSVC-style
# driver flags, not GNU-style ones like -pedantic-errors (see test/
# CMakeLists.txt's identical check).
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(bit_set_is_clang_cl ON)
else()
    set(bit_set_is_clang_cl OFF)
endif()

set(cxx_compile_definitions
    BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
    BOOST_ALL_NO_LIB
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
    # -mno-avx keeps the rest of -march=native and takes 256-bit vectors, and
    # with them every 32-byte stack slot, off the table. -mprefer-vector-width=128
    # is the less blunt alternative if AVX is wanted back on Windows.
    $<$<AND:$<CXX_COMPILER_ID:GNU>,$<PLATFORM_ID:Windows>>:
        -mno-avx
    >
)

get_property(bit_set_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

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

set(current_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(current_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE targets RELATIVE ${current_src_dir} *.cpp)

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
        benchmark::benchmark
        fmt::fmt
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

    # Release only. A benchmark measures the library rather than verifying
    # it, and an unoptimised measurement is meaningless anyway - which is why
    # #32 moved every leg to Release in the first place. Registering it in
    # Debug costs 25 minutes per Windows leg to learn nothing: the MSVC STL's
    # _ITERATOR_DEBUG_LEVEL=2 makes the sieve's repeated flat_set erase about
    # 60x slower, and benchmark.set.sieve then trips ctest's 1500s per-test
    # timeout.
    #
    # Two spellings, because neither works on both generator kinds:
    #
    # - Multi-config (the Visual Studio generator) leaves CMAKE_BUILD_TYPE
    #   empty, so an if() on it would register these in every configuration,
    #   Debug included. CONFIGURATIONS is the only thing that discriminates.
    # - Single-config (Ninja, Makefiles) does set CMAKE_BUILD_TYPE, but
    #   CONFIGURATIONS is matched by ctest against the -C it was given, and
    #   the GCC, Clang and MinGW legs invoke a bare "ctest --test-dir build"
    #   with no -C. There it matches nothing, and the benchmarks would
    #   silently stop running anywhere at all rather than in Debug only.
    if(bit_set_is_multi_config)
        add_test(NAME ${target_id} CONFIGURATIONS Release COMMAND ${target_id})
    elseif(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_test(NAME ${target_id} COMMAND ${target_id})
    endif()
endforeach()
