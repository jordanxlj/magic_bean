include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/tools/include)

link_directories(${PROJECT_SOURCE_DIR}/tools/lib/mac ${PROJECT_SOURCE_DIR}/tools/lib/linux ${PROJECT_BINARY_DIR}/..)

aux_source_directory(. TEST_SRC)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/..)

add_executable(magic_test ${TEST_SRC})
target_link_libraries(magic_test gmock pthread magic)
