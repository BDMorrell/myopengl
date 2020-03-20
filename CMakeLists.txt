cmake_minimum_required(VERSION 3.10)
project(mogl VERSION 0.1)

# Deffinitions
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(MOD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/modules")

# This project
set(SOURCES "${SRC_DIR}/main.cpp" "${SRC_DIR}/getFileContents.cpp")
include_directories(lib)
configure_file(lib/CMakeConfig.hpp.in lib/CMakeConfig.hpp)
add_executable(${PROJECT_NAME} "${SOURCES}")
file(COPY shaders assets DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

# GLM
set(GLM_DIR "${MOD_DIR}/glm")
include_directories("${GLM_DIR}")

# GLFW
set(GLFW_DIR "${MOD_DIR}/glfw")
set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory("${GLFW_DIR}")
include_directories("${GLFW_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${GLFW_LIBRARIES})
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# GLAD
set(GLAD_DIR "${MOD_DIR}/glad")
add_subdirectory("${GLAD_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} PRIVATE "glad" "${CMAKE_DL_LIBS}")

# STB
include_directories("${MOD_DIR}/stb")
link_directories("${MOD_DIR}/stb")
