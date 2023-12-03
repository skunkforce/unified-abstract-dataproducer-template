cmake_minimum_required(VERSION 3.10)

project(UaDI_template VERSION 1.0.0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

set(BUILD_SHARED_LIBS ON)

add_library(UaDI SHARED src/UaDI_template.c)
target_compile_definitions(UaDI PRIVATE UADI_EXPORTS)

install(TARGETS UaDI DESTINATION lib)
install(FILES src/UaDI_template.h DESTINATION include)
