cmake_minimum_required(VERSION 3.28)
project(n_body_simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(MPI REQUIRED)

if (APPLE)
    set(OPENMP_INCLUDE_DIR /opt/homebrew/opt/libomp/include)
    set(OPENMP_LIB_DIR /opt/homebrew/opt/libomp/lib)
else ()
    find_package(OpenMP REQUIRED COMPONENTS CXX)
endif ()

add_subdirectory(external/glfw)

add_subdirectory(external/glew)

add_subdirectory(external/glm)

file(GLOB_RECURSE SOURCES src/source/*.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/source/main.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/source/main-mpi.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/source/main-body-gen.cpp)

add_executable(n_body_simulation
        src/source/main.cpp
        ${SOURCES}
)

add_executable(n_body_simulation_mpi
        src/source/main-mpi.cpp
        ${SOURCES}
)

add_executable(n_body_simulation_body_gen
        src/source/main-body-gen.cpp
        ${SOURCES}
)

function(configure_nbody_target target)
    target_include_directories(${target} PRIVATE
            src/source
            src/source/opengl-framework
            src/include
            src/include/opengl-framework
    )

    target_compile_options(${target} PRIVATE
            -Xpreprocessor -fopenmp
            -I${OPENMP_INCLUDE_DIR}
    )

    if (APPLE)
        target_link_directories(${target} PRIVATE ${OPENMP_LIB_DIR})
        target_link_libraries(${target} PRIVATE omp)
    else ()
        target_link_libraries(${target} PRIVATE OpenMP::OpenMP_CXX)
    endif ()

    target_link_libraries(${target} PRIVATE
            OpenCL::OpenCL
            glfw
            libglew_static
            glm
            MPI::MPI_CXX
    )

    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/shader
            $<TARGET_FILE_DIR:${target}>/shader
    )

    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/src/kernels
            $<TARGET_FILE_DIR:${target}>/kernels
    )
endfunction()

configure_nbody_target(n_body_simulation)
configure_nbody_target(n_body_simulation_mpi)
configure_nbody_target(n_body_simulation_body_gen)