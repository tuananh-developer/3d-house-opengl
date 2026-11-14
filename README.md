# 3D House OpenGL

A small 3D house scene built with **C++17, OpenGL, FreeGLUT, GLU and CMake**. The codebase is split into independent modules for the house model, environment, camera and rendering utilities.

## Features

- Procedural house geometry: walls, gable roof, chimney, door, windows and porch.
- Environment: ground, walkway, flower beds, trees and fence.
- Orbit camera controlled by the mouse, with bounded zoom.
- Perspective and orthographic projection modes.
- Basic fixed-function lighting, materials and depth testing.
- CMake build and a GitHub Actions build workflow.

## Tech stack

| Area | Choice |
|---|---|
| Language | C++17 |
| Rendering API | OpenGL compatibility profile |
| Window and input | FreeGLUT |
| Utility library | GLU |
| Build system | CMake 3.16+ |
| CI | GitHub Actions |

> This project intentionally uses the OpenGL fixed-function pipeline (glBegin, matrix stack, glMaterial, glLight) to keep rendering operations explicit and easy to follow. It does not target the modern OpenGL core profile.

## Getting started

### Fedora

    sudo dnf install gcc-c++ cmake freeglut-devel mesa-libGLU-devel

### Ubuntu / Debian

    sudo apt update
    sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev

### Configure, build and run

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel
    ./build/3d-house-opengl

Run the executable from a desktop session with an OpenGL-capable display. A headless CI runner can compile the project, but it does not provide an interactive window for visual testing.

## Controls

| Input | Action |
|---|---|
| Drag left mouse button | Orbit camera |
| Mouse wheel | Zoom |
| + / = | Zoom in |
| - / _ | Zoom out |
| P | Toggle perspective / orthographic projection |
| L | Toggle lighting |
| R | Reset camera and display options |
| Q / Esc | Quit |

## Repository layout

    .
    ├── .github/
    │   ├── PULL_REQUEST_TEMPLATE.md
    │   └── workflows/build.yml
    ├── docs/
    │   ├── architecture.md
    │   ├── development.md
    │   ├── roadmap.md
    │   └── team-workflow.md
    ├── include/house/
    │   ├── camera.hpp
    │   ├── render_utils.hpp
    │   └── scene.hpp
    ├── src/
    │   ├── camera.cpp
    │   ├── environment.cpp
    │   ├── house.cpp
    │   ├── main.cpp
    │   └── render_utils.cpp
    ├── CMakeLists.txt
    ├── CONTRIBUTING.md
    └── LICENSE

## Documentation

- [Architecture](docs/architecture.md) — modules, render flow, coordinate conventions and design boundaries.
- [Development setup](docs/development.md) — prerequisites, build commands, troubleshooting and code conventions.
- [Team workflow](docs/team-workflow.md) — ownership, branch strategy, reviews and integration rules.
- [Roadmap](docs/roadmap.md) — current scope, planned improvements and technical priorities.
- [Contributing](CONTRIBUTING.md) — contribution checklist and pull request expectations.

## Rendering overview

Vertices are transformed through the model, view and projection stages:

    clipPosition = Projection × View × Model × localPosition

The current implementation uses the OpenGL matrix stack for model transforms, gluLookAt for the view transform, and gluPerspective / glOrtho for projection. See the [architecture guide](docs/architecture.md) for details.

## Contributing

Open a focused feature branch and submit a pull request to main. Keep module ownership clear, build locally before requesting review, and include screenshots for visible scene changes. Read [CONTRIBUTING.md](CONTRIBUTING.md) before starting.

## License

MIT. See [LICENSE](LICENSE).
