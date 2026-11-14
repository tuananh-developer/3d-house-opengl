# 3D House OpenGL

A small 3D house scene built with **C++17, OpenGL, FreeGLUT, GLU and CMake**. The codebase is split into independent modules for the house model, environment, camera and rendering utilities.

## Features

- Procedural house geometry: walls, gable roof, chimney, door, windows and porch.
- Environment: ground, walkway, flower beds, trees and fence.
- Orbit and pan camera with bounded, distance-aware zoom in both projection modes.
- Perspective and orthographic projection modes.
- World-space positional lighting, materials and depth testing.
- Screen-space HUD with projection and lighting status.
- CMake build, camera behavior tests and GitHub Actions CI.

## Tech stack

| Area | Choice |
|---|---|
| Language | C++17 |
| Rendering API | OpenGL compatibility profile |
| Window and input | FreeGLUT |
| Utility library | GLU |
| Build system | CMake 3.16+ |
| CI | GitHub Actions |

> This project intentionally uses the OpenGL fixed-function pipeline to keep rendering operations explicit and easy to follow. It does not target the modern OpenGL core profile.

## Getting started

### Fedora

    sudo dnf install gcc-c++ cmake freeglut-devel mesa-libGLU-devel

### Ubuntu / Debian

    sudo apt update
    sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev

### Configure, build, test and run

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel
    ctest --test-dir build --output-on-failure
    ./build/3d-house-opengl

Run the executable from a desktop session with an OpenGL-capable display. A headless CI runner can compile and test camera behavior, but it does not provide an interactive window for visual testing.

## Controls

| Input | Action |
|---|---|
| Drag left mouse button | Orbit camera |
| Drag right or middle mouse button | Pan camera |
| Mouse wheel | Zoom in / out |
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
    │   ├── branch-protection.md
    │   ├── development.md
    │   ├── roadmap.md
    │   ├── roles-and-permissions.md
    │   └── team-workflow.md
    ├── include/house/
    ├── src/
    ├── tests/
    ├── CMakeLists.txt
    ├── CONTRIBUTING.md
    └── LICENSE

## Documentation

- [Architecture](docs/architecture.md) — modules, render flow and coordinate conventions.
- [Development setup](docs/development.md) — prerequisites, build commands and troubleshooting.
- [Roles and permissions](docs/roles-and-permissions.md) — team responsibilities and repository access model.
- [Team workflow](docs/team-workflow.md) — branch strategy, pull requests and reviews.
- [Branch protection](docs/branch-protection.md) — required GitHub settings to protect main.
- [Roadmap](docs/roadmap.md) — current scope and technical priorities.
- [Contributing](CONTRIBUTING.md) — development and pull request standards.

## Rendering overview

Vertices are transformed through the model, view and projection stages:

    clipPosition = Projection × View × Model × localPosition

The current implementation uses the OpenGL matrix stack for model transforms, gluLookAt for the view transform, and gluPerspective / glOrtho for projection. See [architecture.md](docs/architecture.md) for details.

## Contribution policy

**All changes must be merged through pull requests. Direct pushes to main are not part of the team workflow.** The repository's branch protection must also be enabled in GitHub Settings to enforce this server-side. See [branch-protection.md](docs/branch-protection.md).

## License

MIT. See [LICENSE](LICENSE).
