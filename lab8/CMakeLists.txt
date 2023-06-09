set(EXECUTABLE_NAME lab8)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

set(SOURCE ${SOURCE_DIR}/main.cpp)

add_executable(${EXECUTABLE_NAME} ${SOURCE})
target_compile_options(${EXECUTABLE_NAME} PRIVATE -std=c++17)

target_link_libraries(
  ${EXECUTABLE_NAME}
  PRIVATE
  ${EXECUTABLE_NAME}_utils
)

add_subdirectory(lib)

