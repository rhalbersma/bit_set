cmake_minimum_required(VERSION 3.9)
project(int_set CXX)

set(project_include_dir
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

add_library(int_set INTERFACE)
target_include_directories(int_set INTERFACE ${project_include_dir})
target_compile_features(int_set INTERFACE cxx_std_17)

include(CTest)
add_subdirectory(test)
