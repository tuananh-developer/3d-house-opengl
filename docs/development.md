# Development setup

## Requirements

- A C++17 compiler (GCC or Clang).
- CMake 3.16 or newer.
- OpenGL development libraries.
- FreeGLUT and GLU development libraries.
- A desktop display with an OpenGL-capable driver for interactive runtime testing.

## Fedora

    sudo dnf install gcc-c++ cmake freeglut-devel mesa-libGLU-devel

## Ubuntu / Debian

    sudo apt update
    sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev

## Configure, build and test

From the repository root:

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build --parallel
    ctest --test-dir build --output-on-failure

Run:

    ./build/3d-house-opengl

For an optimized build, use -DCMAKE_BUILD_TYPE=Release. To rebuild after source changes:

    cmake --build build --parallel

To start from a clean build directory:

    rm -rf build
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build --parallel
    ctest --test-dir build --output-on-failure

## Runtime testing

The GitHub Actions workflow builds the application and runs the automated camera behavior tests. Interactive checks still need to be performed in a desktop session.

When changing camera or rendering code, verify:

1. Left-button dragging orbits smoothly and the camera cannot flip over the top or bottom.
2. Right- and middle-button dragging pans the scene without changing its orientation.
3. Mouse-wheel and + / - zoom work in both perspective and orthographic modes and remain bounded.
4. P switches projection modes without breaking the current viewport aspect ratio.
5. L toggles lighting, and the world-space positional light does not appear to move with the camera.
6. R restores the default camera, projection and lighting state.
7. Resizing the window keeps the scene and HUD usable.
8. The HUD remains readable and does not leave depth, blending or lighting state altered.

When changing geometry, verify at least a front view and an angled view. The CI runner is headless and does not replace these visual checks.

## Troubleshooting

### CMake cannot find OpenGL or GLUT

Check that the development packages are installed, not only the runtime libraries. Re-run CMake after installing packages.

### The executable cannot open a display

Run it from a graphical desktop session. A headless CI runner can compile the program but cannot provide the normal interactive window without a virtual display setup.

### The scene appears flat or objects overlap incorrectly

Check that depth testing is enabled and that the window was created with a depth buffer. Review the model-view matrix state and ensure every push has a matching pop.

### A surface looks unexpectedly dark

Check the face normal direction, material values, light position and whether lighting is enabled. Custom geometry should set a normal for each face.

### Geometry disappears after adding a transform

Check matrix mode and matrix-stack balance. Use glPushMatrix / glPopMatrix to isolate per-object transforms.

## Code conventions

- Use C++17 and keep headers under include/house.
- Keep module interfaces small and explicit.
- Use descriptive function and variable names.
- Use glPushMatrix / glPopMatrix around local object transforms.
- Preserve the world coordinate convention documented in architecture.md.
- Do not commit build output, generated binaries or IDE-specific files.
