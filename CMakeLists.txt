cmake_minimum_required(VERSION 3.16)
project(ThreeDHouseOpenGL VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(3d-house-opengl
    src/main.cpp
    src/camera.cpp
    src/environment.cpp
    src/house.cpp
    src/render_utils.cpp
)

target_include_directories(3d-house-opengl PRIVATE include)

target_link_libraries(3d-house-opengl PRIVATE
    OpenGL::GL
    OpenGL::GLU
    GLUT::GLUT
)

if(MSVC)
    target_compile_options(3d-house-opengl PRIVATE /W4)
else()
    target_compile_options(3d-house-opengl PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
    add_executable(camera-tests
        tests/camera_test.cpp
        src/camera.cpp
    )

    target_include_directories(camera-tests PRIVATE include)
    target_link_libraries(camera-tests PRIVATE OpenGL::GLU)

    if(MSVC)
        target_compile_options(camera-tests PRIVATE /W4)
    else()
        target_compile_options(camera-tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME camera-behavior COMMAND camera-tests)
endif()
