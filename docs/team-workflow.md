# Team workflow

This document defines ownership and collaboration rules for the three-person team.

## Responsibilities

| Member | Role | Primary ownership |
|---|---|---|
| Kiệt | House modeling | src/house.cpp — foundation, walls, roof, door, windows, porch and chimney |
| Huy | Environment and scene | src/environment.cpp — ground, path, planting, trees, fence and gate |
| Team lead | Graphics and integration | src/main.cpp, src/camera.cpp, include/house/camera.hpp, CMake and CI |

The team lead coordinates work, reviews pull requests and keeps main buildable. Each contributor owns their implementation and is responsible for testing it. Shared tasks such as bug triage, visual review and integration are collaborative.

## Branches

Recommended branches:

- Kiệt: feature/kiet-house-model
- Huy: feature/huy-environment
- Team lead: feature/team-lead-camera-integration

Create short-lived branches from the latest main. Do not push unfinished work directly to main.

## Pull request flow

1. Pull the latest main.
2. Create or update your feature branch.
3. Implement one focused change.
4. Configure and build locally.
5. Run the application when the change affects rendering.
6. Open a pull request to main with a summary, test steps and screenshots for visual changes.
7. Request a review from another member.
8. Resolve feedback and merge only after required checks pass.

## Module ownership and shared files

Prefer editing only the module assigned to you. Changes to include/house/scene.hpp, CMakeLists.txt or src/main.cpp can affect everyone, so coordinate before changing them. If a new public scene function is needed, agree on its name and signature before implementing it.

## Definition of done

- The change matches the agreed scope.
- The project configures and builds successfully.
- Rendering behavior has been checked where relevant.
- The pull request explains the change and test result.
- Shared interfaces remain compatible.
- At least one teammate has reviewed the change.
