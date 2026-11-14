#pragma once

namespace house {
class Camera {
public:
    static constexpr float kDefaultDistance = 19.0F;

    void reset();
    void orbit(float deltaX, float deltaY);
    void pan(float deltaX, float deltaY);
    void zoom(float steps);
    void apply() const;

    float distance() const noexcept { return distance_; }

private:
    float azimuth_ = 42.0F;
    float elevation_ = 25.0F;
    float distance_ = kDefaultDistance;
    float targetX_ = 0.0F;
    float targetY_ = 2.2F;
    float targetZ_ = 0.0F;
};
}  // namespace house
