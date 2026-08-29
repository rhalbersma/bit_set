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

# TEMPORARY, for the MinGW Debug segfault investigation only. Four tests -
# test.bitset.o1, test.set.o0, test.set.o2, test.set.sieve - die with an
# unreported access violation on every WinLibs rung in Debug, and on no other
# platform or configuration. #48 established that -Wa,-mbig-obj is not the
# cause. This block adds observation, not a remedy: it changes what the build
# and the test run report, not what they do, so the four keep crashing and the
# crash is what gets measured.
#
# Two independent instruments, because there are two ways to run out of stack:
#
#   -fstack-usage    writes a .su file next to every object, listing each
#                    function's frame size. A POST_BUILD step ranks them. This
#                    catches the single-oversized-frame mode - Windows reserves
#                    2 MiB against Linux's 8 MiB, and GCC probes the guard page
#                    through ___chkstk_ms on entry, so one frame over the
#                    reserve faults before its body runs.
#
#   gdb -batch       wraps each test. This catches every mode, and says where.
#                    Boost.Test is the reason we are blind today: on MinGW it
#                    has neither BOOST_SEH_BASED_SIGNAL_HANDLING (that needs
#                    _MSC_VER) nor BOOST_SIGACTION_BASED_SIGNAL_HANDLING (that
#                    needs POSIX sigaction), so execution_monitor falls through
#                    to BOOST_NO_SIGNAL_HANDLING and the process simply dies.
#                    The silence under --output-on-failure is that fallback,
#                    not evidence about when the crash happens.
#
# ___chkstk_ms as the faulting frame means stack exhaustion; a frame in our own
# code, or a vector move against a misaligned slot, means something else.
# Delete both instruments once the four are diagnosed.
set(bit_set_mingw_debug OFF)
if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(bit_set_mingw_debug ON)
    find_program(bit_set_gdb NAMES gdb)
    if(NOT bit_set_gdb)
        message(WARNING "MinGW Debug: gdb not found, tests run unwrapped and will stay silent on crashing")
    endif()
endif()

set(cxx_compile_options_stack_usage
    $<$<BOOL:${bit_set_mingw_debug}>:
        -fstack-usage
    >
)

# The experiment the two instruments above were built to inform, run early
# because it is cheap and because it doubles as the fix. 8 MiB is Linux's
# default, against the 2 MiB mingw-w64 writes into the PE header - so this
# asks, in one CI run, whether the four crash for no reason other than
# having a fifth of the headroom their green Linux counterparts get.
#
# It does not blind the instruments. Green here is itself the answer, and
# red leaves gdb to say what the cause is instead. Widen this to every
# Windows-GNU configuration, not just Debug, if it turns out to be the fix;
# it is scoped to Debug now to keep the experiment one variable.
set(link_options_stack_reserve
    $<$<BOOL:${bit_set_mingw_debug}>:
        -Wl,--stack,8388608
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
        ${cxx_compile_options_stack_usage}
    )

    target_link_options(
        ${target_id} PRIVATE
        ${link_options_stack_reserve}
    )

    if(bit_set_mingw_debug)
        add_custom_command(
            TARGET ${target_id} POST_BUILD
            COMMAND ${CMAKE_COMMAND}
                -DSU_DIR=${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_id}.dir
                -DSU_LABEL=${target_id}
                -DSU_TOP=10
                -P ${CMAKE_CURRENT_SOURCE_DIR}/report_stack_usage.cmake
            VERBATIM
        )
    endif()

    # The dump lives in on_crash.gdb rather than in -ex flags here, because it
    # has to be guarded: gdb -batch exits non-zero if any scripted command
    # errors, and that overrides --return-child-result. Unguarded "bt" after a
    # clean exit errors with "No stack.", which reported every passing test to
    # ctest as a failure - 18 of 27, where only a handful are real crashes.
    if(bit_set_mingw_debug AND bit_set_gdb)
        add_test(
            NAME ${target_id}
            COMMAND ${bit_set_gdb} -batch -nx
                -x ${CMAKE_CURRENT_SOURCE_DIR}/on_crash.gdb
                --return-child-result
                --args $<TARGET_FILE:${target_id}>
        )
    else()
        add_test(${target_id} ${target_id})
    endif()
endforeach()
