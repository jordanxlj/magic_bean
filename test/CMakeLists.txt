include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/tools/include)

link_directories(${PROJECT_SOURCE_DIR}/tools/lib/mac ${PROJECT_SOURCE_DIR}/tools/lib/linux ${PROJECT_BINARY_DIR}/..)

set(TEST_SRC test_event_poller.cc test_batch_event_processor.cc test_sequencer.cc test_ring_buffer.cc test_main.cc)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/..)

add_executable(magic_test ${TEST_SRC})
target_link_libraries(magic_test gmock pthread magic)
