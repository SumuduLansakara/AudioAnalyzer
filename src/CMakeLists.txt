include_directories(${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(aulog ${SRC_FILES})
target_link_libraries(aulog portaudio)
