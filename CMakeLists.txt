cmake_minimum_required(VERSION 3.11)
project(inrol_sim_cablewinding)

include_directories(
include 
${Eigen_INCLUDE_PATH} ${GLEW_INCLUDE_PATH} ${GLM_INCLUDE_PATH} ${GLFW_INCLUDE_PATH}
)
link_directories(external)
add_executable(inrol_sim_cablewinding src/main.cpp)
target_link_libraries(
inrol_sim_cablewinding 
cable_benchmark 
${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARIES}
)

