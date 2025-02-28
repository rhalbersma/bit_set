#          Copyright Rein Halbersma 2014-2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED)
find_package(benchmark REQUIRED)
find_package(fmt REQUIRED)

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
        -pedantic-errors
    >
    $<$<CXX_COMPILER_ID:Clang>:
        -Weverything
        -Wno-unknown-warning-option         # forward compatibility for new Clang warnings
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-c++20-compat
        -Wno-c++20-compat-pedantic
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
)

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

    add_test(${target_id} ${target_id})
endforeach()
