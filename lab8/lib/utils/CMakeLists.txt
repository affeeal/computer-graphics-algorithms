set(LIBRARY_NAME ${EXECUTABLE_NAME}_utils)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB SOURCES ${SOURCE_DIR}/*.cpp)

add_library(${LIBRARY_NAME} STATIC ${SOURCES})
target_include_directories(${LIBRARY_NAME} PUBLIC ${INCLUDE_DIR})

target_link_libraries(
  ${LIBRARY_NAME}
  PRIVATE
  OpenGL::GL
  glfw
  glad
  glm
  stb_image
)
  
