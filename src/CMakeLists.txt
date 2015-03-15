include_directories(${PROJECT_SOURCE_DIR}/include)
set(SRC hello.cc)
add_library(libmagic SHARED ${SRC})
