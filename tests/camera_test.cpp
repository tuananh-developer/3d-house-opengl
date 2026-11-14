#include "house/camera.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

namespace {
void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

void expectNear(float actual, float expected, float tolerance,
                const std::string& message) {
    expect(std::fabs(actual - expected) <= tolerance, message);
}
}  // namespace

int main() {
    house::Camera camera;

    expectNear(camera.distance(), house::Camera::kDefaultDistance, 0.0001F,
               "camera starts at its default distance");

    camera.zoom(-1.0F);
    const float zoomedInDistance = camera.distance();
    expect(zoomedInDistance < house::Camera::kDefaultDistance,
           "negative zoom steps move the camera closer");

    camera.zoom(1.0F);
    expectNear(camera.distance(), house::Camera::kDefaultDistance, 0.0001F,
               "one zoom-in step followed by one zoom-out step is reversible");

    camera.zoom(-1000.0F);
    expectNear(camera.distance(), 7.0F, 0.0001F,
               "zoom-in is clamped to the minimum distance");

    camera.zoom(1000.0F);
    expectNear(camera.distance(), 42.0F, 0.0001F,
               "zoom-out is clamped to the maximum distance");

    camera.zoom(std::numeric_limits<float>::quiet_NaN());
    expectNear(camera.distance(), 42.0F, 0.0001F,
               "non-finite zoom input is ignored");

    camera.reset();
    expectNear(camera.distance(), house::Camera::kDefaultDistance, 0.0001F,
               "reset restores the default distance");

    camera.orbit(120.0F, 5000.0F);
    camera.pan(25.0F, -40.0F);
    expectNear(camera.distance(), house::Camera::kDefaultDistance, 0.0001F,
               "orbiting and panning do not change camera distance");

    std::cout << "All camera behavior tests passed.\n";
    return EXIT_SUCCESS;
}
