# Contributing

Thanks for contributing to 3D House OpenGL. The project is intentionally small and modular; keep changes focused and avoid coupling unrelated scene modules.

## Development setup

Install the dependencies for your platform and build the project using the commands in [docs/development.md](docs/development.md).

    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --parallel

## Branch naming

Use one branch per change:

- feature/<short-description>
- fix/<short-description>
- docs/<short-description>
- refactor/<short-description>

Examples:

- feature/kiet-house-model
- feature/huy-environment
- feature/camera-improvements

Do not commit directly to main.

## Commit messages

Use a concise conventional format:

- feat: add roof details
- fix: clamp camera zoom
- docs: clarify build instructions
- refactor: extract window geometry

## Pull request checklist

- [ ] The change has a clear, focused scope.
- [ ] The project configures and builds successfully.
- [ ] Visible rendering changes have screenshots.
- [ ] Shared headers and coordinate conventions remain compatible.
- [ ] Documentation is updated when behavior or setup changes.
- [ ] At least one other team member has reviewed the change.

Describe what changed, why it changed, how it was tested, and whether another module needs to be updated.

## Avoiding merge conflicts

- House geometry belongs in src/house.cpp.
- Environment geometry belongs in src/environment.cpp.
- Camera behavior belongs in src/camera.cpp and include/house/camera.hpp.
- Window callbacks, global scene state and rendering orchestration belong in src/main.cpp.
- Coordinate shared API changes before editing include/house/scene.hpp.

If a change needs to cross module boundaries, discuss the interface first and keep the shared-header change as small as possible.
