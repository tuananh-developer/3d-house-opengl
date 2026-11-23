cmake_minimum_required(VERSION 3.16)
project(ModernLandscape3D VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Tìm các thư viện cần thiết
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 QUIET)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SOURCES
    src/main.cpp
    src/glad.c
)

add_executable(modern_landscape ${SOURCES})

target_compile_options(modern_landscape PRIVATE -Wall -Wextra -Wpedantic -O2)

if(glfw3_FOUND)
    target_link_libraries(modern_landscape PRIVATE
        glfw
        OpenGL::GL
        ${CMAKE_DL_LIBS}
    )
    message(STATUS "Configured with GLFW 3.3+ Core Profile")
else()
    # Fallback to FreeGLUT modern 3.3 core if GLFW is not found
    find_package(GLUT REQUIRED)
    target_compile_definitions(modern_landscape PRIVATE USE_GLUT)
    target_link_libraries(modern_landscape PRIVATE
        GLUT::GLUT
        OpenGL::GL
        ${CMAKE_DL_LIBS}
    )
    message(STATUS "GLFW not found, falling back to FreeGLUT Modern 3.3 Core Profile")
endif()

