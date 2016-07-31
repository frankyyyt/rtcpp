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

add_executable(rt_bst src/tests/rt_bst.cpp)
add_executable(rt_set src/tests/rt_set.cpp)
add_executable(rt_list src/tests/rt_list.cpp)
add_executable(rt_forward_list src/tests/rt_forward_list.cpp)
add_executable(rt_align src/tests/rt_align.cpp)
add_executable(rt_reverse src/tests/rt_reverse.cpp)
add_executable(rt_alloc src/tests/rt_alloc.cpp)
add_executable(rt_matrix src/tests/rt_matrix.cpp)
add_executable(rt_comp_counting_sort src/tests/rt_comp_counting_sort.cpp)
add_executable(rt_dist_counting_sort src/tests/rt_dist_counting_sort.cpp)
add_executable(rt_insertion_sort src/tests/rt_insertion_sort.cpp)
add_executable(rt_bubble_sort src/tests/rt_bubble_sort.cpp)
add_executable(rt_straight_selection_sort src/tests/rt_straight_selection_sort.cpp)
add_executable(rt_binary_search src/tests/rt_binary_search.cpp)
add_executable(rt_find_intrusive src/tests/rt_find_intrusive.cpp)
add_executable(rt_node_stack src/tests/rt_node_stack.cpp)
add_executable(rt_node src/tests/rt_node.cpp)
add_executable(ex_alloc_list src/examples/alloc_example_list.cpp)
add_executable(ex_alloc_scoped src/examples/alloc_example_scoped.cpp)
add_executable(ex_alloc_set src/examples/alloc_example_set.cpp)
add_executable(ex_alloc_unordered_set src/examples/alloc_example_unordered_set.cpp)
add_executable(ex_matrix src/examples/matrix_example1.cpp)
add_executable(bench_list_frag src/benchmarks/bench_list_frag.cpp)
add_executable(bench_find src/benchmarks/bench_find.cpp)
add_executable(bench_set src/benchmarks/bench_set.cpp)
add_executable(bench_rtset src/benchmarks/bench_rtset.cpp)
add_executable(bench_list src/benchmarks/bench_list.cpp)
add_executable(bench_stack src/benchmarks/bench_stack.cpp)

if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
  target_link_libraries(bench_set ${Boost_LIBRARIES})
endif()

add_test(NAME rt_bst COMMAND rt_bst)
add_test(NAME rt_set COMMAND rt_set)
add_test(NAME rt_list COMMAND rt_list)
add_test(NAME rt_node COMMAND rt_node)
add_test(NAME rt_align COMMAND rt_align)
add_test(NAME rt_alloc COMMAND rt_alloc)
add_test(NAME rt_matrix COMMAND rt_matrix)
add_test(NAME rt_reverse COMMAND rt_reverse)
add_test(NAME rt_node_stack COMMAND rt_node_stack)
add_test(NAME rt_bubble_sort COMMAND rt_bubble_sort)
add_test(NAME rt_forward_list COMMAND rt_forward_list)
add_test(NAME rt_binary_search COMMAND rt_binary_search)
add_test(NAME rt_insertion_sort COMMAND rt_insertion_sort)
add_test(NAME rt_find_intrusive COMMAND rt_find_intrusive)
add_test(NAME rt_comp_counting_sort COMMAND rt_comp_counting_sort)
add_test(NAME rt_dist_counting_sort COMMAND rt_dist_counting_sort)
add_test(NAME rt_straight_selection_sort COMMAND rt_straight_selection_sort)
add_test(NAME ex_matrix COMMAND ex_matrix)
add_test(NAME ex_alloc_set COMMAND ex_alloc_set)
add_test(NAME ex_alloc_list COMMAND ex_alloc_list)
add_test(NAME ex_alloc_scoped COMMAND ex_alloc_scoped)
add_test(NAME ex_alloc_unordered_set COMMAND ex_alloc_unordered_set)

