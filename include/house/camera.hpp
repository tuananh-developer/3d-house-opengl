#pragma once

namespace house {
class Camera {
public:
    void reset();
    void orbit(float deltaX, float deltaY);
    void zoom(float steps);
    void apply() const;
private:
    float azimuth_ = 42.0F;
    float elevation_ = 25.0F;
    float distance_ = 19.0F;
    float targetY_ = 2.2F;
};
}  // namespace house
