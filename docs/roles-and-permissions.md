# Roles and permissions

This document separates **engineering responsibility** from **GitHub repository permission**. A person may own a module without having permission to bypass the protected branch.

## Team roles

| Member | Engineering role | Owns / leads | Expected outputs |
|---|---|---|---|
| Kiệt | 3D House Modeling | src/house.cpp | House geometry, roof, facade, door, windows, porch, visual checks |
| Huy | Environment & Scene | src/environment.cpp | Ground, yard, paths, trees, planting, fence and gate |
| Team lead (tuananh-developer) | Graphics Lead & Integrator | src/main.cpp, src/camera.cpp, camera interface, CMake and CI | Camera, projection, lighting integration, code review, release stability and technical coordination |

## Role responsibilities

### Kiệt — House Modeling
- Implement and maintain the architectural geometry module.
- Keep the facade facing negative Z and follow the shared world-coordinate convention.
- Reuse shared rendering utilities instead of duplicating primitives.
- Test the house from front, side and orbit camera views.
- Open focused pull requests and respond to review feedback.

### Huy — Environment & Scene
- Implement and maintain environment geometry.
- Keep paths, planting, trees and fences aligned with the ground plane.
- Ensure environment objects do not obscure the house or introduce obvious z-fighting.
- Test the scene from multiple camera angles.
- Open focused pull requests and respond to review feedback.

### Team lead — Graphics Lead & Integrator
- Own camera controls, projection, lighting state, input and render orchestration.
- Maintain CMake and GitHub Actions build workflow.
- Review pull requests and coordinate changes to shared headers.
- Keep main buildable and resolve integration conflicts.
- Prioritize issues and maintain the architecture and developer documentation.
- Ensure decisions affecting multiple modules are discussed before implementation.

## Shared responsibilities

Everyone is expected to:
- Build and run the project before requesting review.
- Explain changes made in their own pull requests.
- Review at least one teammate's pull request when requested.
- Help investigate integration regressions.
- Keep commits truthful and attributable to the actual contributor.

## Suggested GitHub permission model

Use the least privilege needed for normal development:

| Member | Suggested GitHub repository permission | Reason |
|---|---|---|
| Kiệt | Write | Push feature branches and open pull requests |
| Huy | Write | Push feature branches and open pull requests |
| Team lead | Admin / Maintain | Manage project settings, review work and maintain integration |

Do not grant Admin merely because someone owns a code module. The team lead currently owns the repository; Kiệt and Huy's GitHub usernames are needed before their collaborator permissions can be assigned.

## Approval and merge authority

- Every change enters main through a pull request.
- The pull request author cannot approve their own pull request.
- At least one other team member approves.
- Required CI checks must pass.
- The team lead is responsible for confirming integration readiness and merging.
- Repository administrators also follow the pull-request policy; branch protection should not include an administrator bypass.

The server-side enforcement settings are documented in [branch-protection.md](branch-protection.md).
