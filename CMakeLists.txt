cmake_minimum_required(VERSION 3.28)
project(open_mp_intro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OPENMP_INCLUDE_DIR /opt/homebrew/opt/libomp/include)
set(OPENMP_LIB_DIR /opt/homebrew/opt/libomp/lib)

add_executable(open_mp_intro
        main.cpp
)

target_compile_options(open_mp_intro PRIVATE
        -Xpreprocessor -fopenmp
        -I${OPENMP_INCLUDE_DIR}
)

target_link_directories(open_mp_intro PRIVATE ${OPENMP_LIB_DIR})
target_link_libraries(open_mp_intro PRIVATE omp)