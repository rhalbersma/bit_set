cmake_minimum_required(VERSION 3.15)
project(bit_set CXX)

set(project_include_dir
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${project_include_dir})
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

include(CTest)
add_subdirectory(test)
