# Using a single CMakeLists.txt to avoid recursive makefiles.

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(rtcpp)

find_package(Boost "1.57.0" COMPONENTS container)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
 set(GNU_FOUND true)
endif()

configure_file(
  ${PROJECT_SOURCE_DIR}/src/config.h.in
  ${PROJECT_BINARY_DIR}/config.h
)

enable_testing()
include(CTest)
include(CPack)

include_directories(include)
include_directories(${PROJECT_BINARY_DIR})

install( DIRECTORY ${PROJECT_SOURCE_DIR}/include/rtcpp
         DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

add_executable(rt_set src/tests/rt_set.cpp)
add_executable(rt_list src/tests/rt_list.cpp)
add_executable(rt_forward_list src/tests/rt_forward_list.cpp)
add_executable(rt_align src/tests/rt_align.cpp)
add_executable(rt_matrix src/tests/rt_matrix.cpp)
add_executable(rt_sort src/tests/rt_sort.cpp)
add_executable(rt_algorithm src/tests/rt_algorithm.cpp)
add_executable(rt_node_stack src/tests/rt_node_stack.cpp)
add_executable(ex_alloc_set src/examples/ex_alloc_set.cpp)
add_executable(ex_matrix src/examples/matrix_example1.cpp)
add_executable(bench_find src/benchmarks/bench_find.cpp)
add_executable(bench_set src/benchmarks/bench_set.cpp)
add_executable(bench_list src/benchmarks/bench_list.cpp)
add_executable(bench_stack src/benchmarks/bench_stack.cpp)

if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
  target_link_libraries(bench_set ${Boost_LIBRARIES})
endif()

add_test(NAME rt_set COMMAND rt_set)
add_test(NAME rt_list COMMAND rt_list)
add_test(NAME rt_sort COMMAND rt_sort)
add_test(NAME rt_align COMMAND rt_align)
add_test(NAME rt_matrix COMMAND rt_matrix)
add_test(NAME rt_algorithm COMMAND rt_algorithm)
add_test(NAME rt_node_stack COMMAND rt_node_stack)
add_test(NAME rt_forward_list COMMAND rt_forward_list)
add_test(NAME ex_matrix COMMAND ex_matrix)
add_test(NAME ex_alloc_set COMMAND ex_alloc_set)

