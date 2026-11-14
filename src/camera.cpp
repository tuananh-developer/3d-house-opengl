#include "house/camera.hpp"

#include <algorithm>
#include <cmath>
#include <GL/glu.h>

namespace house {
namespace {
constexpr float kPi = 3.14159265358979323846F;
constexpr float kOrbitSensitivityX = 0.35F;
constexpr float kOrbitSensitivityY = 0.25F;
constexpr float kPanSensitivity = 0.0022F;
constexpr float kMinimumDistance = 7.0F;
constexpr float kMaximumDistance = 42.0F;
}  // namespace

void Camera::reset() {
    azimuth_ = 42.0F;
    elevation_ = 25.0F;
    distance_ = 19.0F;
    targetX_ = 0.0F;
    targetY_ = 2.2F;
    targetZ_ = 0.0F;
}

void Camera::orbit(float deltaX, float deltaY) {
    azimuth_ = std::fmod(azimuth_ + deltaX * kOrbitSensitivityX, 360.0F);
    if (azimuth_ < 0.0F) {
        azimuth_ += 360.0F;
    }

    elevation_ = std::clamp(
        elevation_ + deltaY * kOrbitSensitivityY, 8.0F, 78.0F);
}

void Camera::pan(float deltaX, float deltaY) {
    const float azimuth = azimuth_ * kPi / 180.0F;
    const float elevation = elevation_ * kPi / 180.0F;

    // Camera-right and camera-up vectors expressed in world coordinates.
    const float rightX = std::cos(azimuth);
    const float rightZ = -std::sin(azimuth);
    const float upX = -std::sin(azimuth) * std::sin(elevation);
    const float upY = std::cos(elevation);
    const float upZ = -std::cos(azimuth) * std::sin(elevation);

    // Scale panning with distance so the same mouse movement feels consistent.
    const float scale = distance_ * kPanSensitivity;
    const float horizontal = deltaX * scale;
    const float vertical = deltaY * scale;

    // Move the camera target opposite the pointer's horizontal movement.
    targetX_ -= rightX * horizontal;
    targetZ_ -= rightZ * horizontal;

    // A downward pointer movement should move the scene down on screen.
    targetX_ += upX * vertical;
    targetY_ += upY * vertical;
    targetZ_ += upZ * vertical;

    targetX_ = std::clamp(targetX_, -18.0F, 18.0F);
    targetY_ = std::clamp(targetY_, -1.0F, 12.0F);
    targetZ_ = std::clamp(targetZ_, -18.0F, 18.0F);
}

void Camera::zoom(float steps) {
    // Ignore invalid input and bound the exponent to keep the scale positive
    // and finite even if a caller supplies an unusually large step count.
    if (!std::isfinite(steps)) {
        return;
    }

    const float boundedSteps = std::clamp(steps, -20.0F, 20.0F);
    const float factor = std::exp(-boundedSteps * 0.12F);
    distance_ = std::clamp(
        distance_ * factor, kMinimumDistance, kMaximumDistance);
}

void Camera::apply() const {
    const float azimuth = azimuth_ * kPi / 180.0F;
    const float elevation = elevation_ * kPi / 180.0F;
    const float horizontal = distance_ * std::cos(elevation);

    const float eyeX = targetX_ + horizontal * std::sin(azimuth);
    const float eyeY = targetY_ + distance_ * std::sin(elevation);
    const float eyeZ = targetZ_ + horizontal * std::cos(azimuth);

    gluLookAt(
        eyeX, eyeY, eyeZ,
        targetX_, targetY_, targetZ_,
        0.0, 1.0, 0.0);
}
}  // namespace house
