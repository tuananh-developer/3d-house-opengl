# Contributing

Thanks for contributing to 3D House OpenGL. The project is intentionally small and modular; keep changes focused and avoid coupling unrelated scene modules.

## Main branch policy

**Do not push directly to main. All changes must enter main through a pull request.**

1. Create a feature, fix or docs branch from the latest main.
2. Commit your changes on that branch.
3. Push the branch to GitHub.
4. Open a pull request targeting main.
5. Wait for CI and at least one approval from another team member.
6. Resolve review comments and merge the pull request.

Do not use force-push on main. Do not merge your own pull request without the required review. Repository administrators follow the same process.

> This is the team policy. GitHub must also have a branch protection rule or repository ruleset for main to prevent direct pushes at the server level. Follow [docs/branch-protection.md](docs/branch-protection.md).

## Development setup

Install the dependencies for your platform and build the project using [docs/development.md](docs/development.md).

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
- [ ] CI checks have passed.

Describe what changed, why it changed, how it was tested, and whether another module needs to be updated.

## Module ownership

- House geometry belongs in src/house.cpp.
- Environment geometry belongs in src/environment.cpp.
- Camera behavior belongs in src/camera.cpp and include/house/camera.hpp.
- Window callbacks, global scene state and rendering orchestration belong in src/main.cpp.
- Coordinate shared API changes before editing include/house/scene.hpp.

See [docs/roles-and-permissions.md](docs/roles-and-permissions.md) for responsibility boundaries and [docs/team-workflow.md](docs/team-workflow.md) for the collaboration process.
