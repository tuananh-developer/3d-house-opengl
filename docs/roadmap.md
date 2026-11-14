# Roadmap

The roadmap describes the intended technical direction. Items are proposals until implemented and merged.

## Current baseline

- [x] Modular C++17 / OpenGL application structure.
- [x] Procedural house and environment geometry.
- [x] Orbit camera and zoom controls.
- [x] Perspective / orthographic projection toggle.
- [x] Basic fixed-function lighting and depth testing.
- [x] CMake build and Linux CI compile check.

## Near-term improvements

- [ ] Improve roof shape and add roof tile details.
- [ ] Add more consistent trim and window framing.
- [ ] Refine fence layout and add a front gate.
- [ ] Add camera presets for front, side and isometric views.
- [ ] Separate scene configuration (colors, dimensions and light settings) from drawing code.
- [ ] Add runtime visual regression screenshots or a repeatable manual review checklist.

## Longer-term ideas

- [ ] Add simple interior room geometry.
- [ ] Add a roof visibility toggle to inspect the interior.
- [ ] Add textures for walls, roof and ground.
- [ ] Add multiple lighting presets (day / evening).
- [ ] Evaluate a modern OpenGL renderer using shaders and buffer objects.

## Technical priorities

1. Keep the build reproducible.
2. Avoid merging features that break existing camera controls or scene modules.
3. Improve visual clarity before adding a large number of small decorative objects.
4. Treat a modern OpenGL migration as a separate milestone with its own design and implementation plan.
