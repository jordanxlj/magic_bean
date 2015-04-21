include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/perf)

link_directories(${PROJECT_BINARY_DIR}/..)

aux_source_directory(. PERF_SRC)
aux_source_directory(raw RAW_SRC)
list(APPEND PERF_SRC ${RAW_SRC})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/..)

add_executable(magic_perf ${PERF_SRC})
target_link_libraries(magic_perf pthread magic)
