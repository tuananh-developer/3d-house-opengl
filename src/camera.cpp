#include "house/camera.hpp"
#include <algorithm>
#include <cmath>
#include <GL/glu.h>

namespace house {
namespace { constexpr float kPi = 3.14159265358979323846F; }

void Camera::reset() {
    azimuth_ = 42.0F;
    elevation_ = 25.0F;
    distance_ = 19.0F;
    targetY_ = 2.2F;
}
void Camera::orbit(float deltaX, float deltaY) {
    azimuth_ += deltaX * 0.45F;
    elevation_ = std::clamp(elevation_ + deltaY * 0.35F, 8.0F, 78.0F);
}
void Camera::zoom(float steps) {
    distance_ = std::clamp(distance_ + steps, 8.0F, 38.0F);
}
void Camera::apply() const {
    const float a = azimuth_ * kPi / 180.0F;
    const float e = elevation_ * kPi / 180.0F;
    const float horizontal = distance_ * std::cos(e);
    const float eyeX = horizontal * std::sin(a);
    const float eyeY = targetY_ + distance_ * std::sin(e);
    const float eyeZ = horizontal * std::cos(a);
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, targetY_, 0.0, 0.0, 1.0, 0.0);
}
}  // namespace house
