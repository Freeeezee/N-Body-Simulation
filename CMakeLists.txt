cmake_minimum_required(VERSION 3.28)
project(n_body_simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENMP_INCLUDE_DIR /opt/homebrew/opt/libomp/include)
set(OPENMP_LIB_DIR /opt/homebrew/opt/libomp/lib)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(external/glfw)

add_subdirectory(external/glew)

add_subdirectory(external/glm)

file(GLOB_RECURSE SOURCES src/source/*.cpp)

add_executable(n_body_simulation ${SOURCES})

add_custom_command(TARGET n_body_simulation POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/shader"
        "$<TARGET_FILE_DIR:n_body_simulation>/shader"
)

target_include_directories(n_body_simulation PRIVATE
        src/source
        src/source/opengl-framework
        src/include
        src/include/opengl-framework
)

target_compile_options(n_body_simulation PRIVATE
        -Xpreprocessor -fopenmp
        -I${OPENMP_INCLUDE_DIR}
)

target_link_directories(n_body_simulation PRIVATE ${OPENMP_LIB_DIR})
target_link_libraries(n_body_simulation PRIVATE omp)

target_link_libraries(n_body_simulation PRIVATE OpenCL::OpenCL)

target_link_libraries(n_body_simulation PRIVATE glfw libglew_static glm)