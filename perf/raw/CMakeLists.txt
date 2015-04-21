include_directories(${PROJECT_SOURCE_DIR})

set(PERF_SRC abstract_perf_test_disruptor.cc main.cc)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/..)

add_executable(magic_perf ${PERF_SRC})
target_link_libraries(magic_perf pthread magic)
