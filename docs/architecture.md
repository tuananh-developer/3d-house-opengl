# Architecture

## Overview

The application is a single-window OpenGL scene. FreeGLUT owns the event loop and forwards display, resize, keyboard and mouse events to callbacks in src/main.cpp. The scene is drawn using the OpenGL compatibility profile.

The code is separated by responsibility:

| Module | Responsibility |
|---|---|
| main.cpp | Application entry point, window, callbacks, scene orchestration, projection, lighting and HUD |
| camera.cpp | Orbit/pan camera state, zoom limits and view matrix |
| house.cpp | House geometry: foundation, walls, roof, doors, windows, porch and chimney |
| environment.cpp | Ground, path, flower beds, trees and fence |
| render_utils.cpp | Reusable box geometry and material setup |
| include/house/*.hpp | Public interfaces shared between modules |

## Frame lifecycle

1. FreeGLUT calls display().
2. Color and depth buffers are cleared.
3. Projection matrix is configured from the current window aspect ratio and projection mode.
4. Model-view matrix is reset and the camera applies gluLookAt.
5. The world-space light position is submitted after the view transform is active.
6. Environment and house modules draw their geometry.
7. The HUD is rendered in screen-space coordinates while isolating temporary OpenGL state.
8. glutSwapBuffers() presents the completed frame.

## Coordinate conventions

- World up: positive Y.
- Horizontal right: positive X.
- House front facade: negative Z.
- House origin: approximately the center of its foundation.
- Ground plane: close to Y = 0.

Keep these conventions stable. A module should not silently rotate the world or redefine the front direction.

## Transformations and camera

The conceptual vertex transform is:

    clipPosition = Projection × View × Model × localPosition

- Model transform positions an object in world space. The current helpers use glPushMatrix, glTranslatef and glScalef.
- View transform positions the scene relative to the observer. Camera::apply() computes an orbit position and calls gluLookAt.
- Projection maps camera-space geometry to clip space. The application switches between gluPerspective and glOrtho.

The camera stores azimuth, elevation, distance and a target point. Left-button dragging changes azimuth/elevation, while right- or middle-button dragging moves the target in the camera's screen plane. Mouse-wheel and keyboard input change distance exponentially. The distance is clamped, and orthographic projection derives its view extent from the same distance so zoom remains functional in both projection modes. Elevation and target bounds are also clamped to keep the scene usable.

## Rendering details

- Double buffering avoids drawing directly to the visible front buffer.
- The depth buffer and GL_DEPTH_TEST allow nearer surfaces to occlude farther surfaces.
- Fixed-function lighting uses ambient, diffuse and specular terms.
- The light position is specified after the view transform on each frame. OpenGL transforms it into eye coordinates at submission time, so the positional source remains fixed in world space while the camera moves.
- Materials are set through glMaterial*; GL_NORMALIZE keeps transformed normals normalized after scaling.
- The HUD temporarily switches to orthographic screen coordinates. It saves and restores the matrix stacks and relevant OpenGL state so its overlay does not leave depth, blending or lighting settings altered for the next frame.
- Procedural primitives are used instead of external 3D assets.

## Design boundaries

This project currently targets the OpenGL compatibility pipeline. Keep rendering code consistent with that target unless the team explicitly agrees to a renderer migration. A move to shaders, VAOs and VBOs would be a separate architectural change, not a small refactor.

## Extension points

- Add scene objects in the appropriate module and expose only required functions in scene.hpp.
- Add camera behavior in Camera, keeping input callbacks thin.
- Add shared geometry helpers only when at least two modules need them.
- Keep application state and FreeGLUT callbacks in main.cpp; avoid putting application globals in geometry modules.
