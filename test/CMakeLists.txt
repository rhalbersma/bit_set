find_package(Boost COMPONENTS
    unit_test_framework
)

set(cxx_compile_definitions
    BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE
    BOOST_ALL_NO_LIB
    BOOST_ALL_DYN_LINK
    BOOST_TEST_MAIN
    $<$<CXX_COMPILER_ID:MSVC>:
        _SCL_SECURE_NO_WARNINGS
    >
)

set(cxx_compile_options_warnings
    $<$<CXX_COMPILER_ID:MSVC>:
        /W4
    >
    $<$<CXX_COMPILER_ID:Clang>:
        -Weverything
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-disabled-macro-expansion
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-range-loop-analysis
    >
    $<$<CXX_COMPILER_ID:GNU>:
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
        -Wsign-compare
        -Wsign-conversion
        -Wsign-promo
    >
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -Werror
        -pedantic-errors
        -Wno-padded
    >
)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(cxx_compile_options_coverage
        -g
        --coverage
    )

    set(cxx_linker_options_coverage
        --coverage
    )
endif()

set(test_include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(test_source_dir ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE targets RELATIVE ${test_source_dir} *.cpp)

foreach(t ${targets})
    get_filename_component(target_path ${t} PATH)
    get_filename_component(target_name_we ${t} NAME_WE)
    string(REPLACE "/" "." target_id ${target_path}/${target_name_we})
    string(REGEX REPLACE "^[.]" "" target_id ${target_id})

    add_executable(${target_id} src/${t})

    target_link_libraries(${target_id} PRIVATE
        int_set
        Boost::unit_test_framework
        ${cxx_linker_options_coverage}
    )

    target_include_directories(${target_id} PRIVATE
        ${test_include_dir}
    )

    target_compile_definitions(${target_id} PRIVATE
        ${cxx_compile_definitions}
    )

    target_compile_options(${target_id} PRIVATE
        ${cxx_compile_options_warnings}
        ${cxx_compile_options_coverage}
    )

    add_test(${target_id} ${target_id})
endforeach()
