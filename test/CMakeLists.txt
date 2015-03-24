include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/tools/include)
set(SRC test_event_poller.cc)
add_executable(magic ${SRC})
