include_directories(${PROJECT_SOURCE_DIR}/include)
set(SRC sequence.cc)
add_library(libmagic SHARED ${SRC})
