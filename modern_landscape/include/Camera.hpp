#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace modern {

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum class CameraMode {
    FIRST_PERSON, // Tự do bay lượn / đi bộ (WASD + Chuột)
    ORBIT         // Xoay quanh tâm biệt thự (0,0,0)
};

/**
 * @brief Class Camera hỗ trợ cả chế độ First-Person (bay/đi bộ) và Orbit Camera
 */
class Camera {
public:
    // Thuộc tính góc nhìn
    glm::vec3 Position{-2.0f, 7.0f, 27.0f};
    glm::vec3 Front{0.0f, -0.25f, -0.97f};
    glm::vec3 Up{0.0f, 1.0f, 0.0f};
    glm::vec3 Right{1.0f, 0.0f, 0.0f};
    glm::vec3 WorldUp{0.0f, 1.0f, 0.0f};

    // Góc Euler
    float Yaw{-90.0f};
    float Pitch{-14.0f};

    // Tùy chọn di chuyển
    float MovementSpeed{12.0f};
    float MouseSensitivity{0.12f};
    float Zoom{45.0f};

    // Thuộc tính cho chế độ Orbit
    CameraMode Mode{CameraMode::FIRST_PERSON};
    glm::vec3 OrbitTarget{0.0f, 2.5f, 5.0f};
    float OrbitRadius{32.0f};
    float OrbitAzimuth{35.0f};
    float OrbitElevation{28.0f};

    Camera(glm::vec3 position = glm::vec3(-2.0f, 7.0f, 27.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = -14.0f)
        : Position(position), WorldUp(up), Yaw(yaw), Pitch(pitch) {
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix() const {
        if (Mode == CameraMode::FIRST_PERSON) {
            return glm::lookAt(Position, Position + Front, Up);
        } else {
            // Tính toán vị trí Orbit theo tọa độ cầu (Spherical Coordinates)
            float radAz = glm::radians(OrbitAzimuth);
            float radEl = glm::radians(OrbitElevation);
            glm::vec3 orbitPos(
                OrbitTarget.x + OrbitRadius * std::cos(radEl) * std::sin(radAz),
                OrbitTarget.y + OrbitRadius * std::sin(radEl),
                OrbitTarget.z + OrbitRadius * std::cos(radEl) * std::cos(radAz)
            );
            return glm::lookAt(orbitPos, OrbitTarget, WorldUp);
        }
    }

    glm::vec3 GetEyePosition() const {
        if (Mode == CameraMode::FIRST_PERSON) {
            return Position;
        } else {
            float radAz = glm::radians(OrbitAzimuth);
            float radEl = glm::radians(OrbitElevation);
            return glm::vec3(
                OrbitTarget.x + OrbitRadius * std::cos(radEl) * std::sin(radAz),
                OrbitTarget.y + OrbitRadius * std::sin(radEl),
                OrbitTarget.z + OrbitRadius * std::cos(radEl) * std::cos(radAz)
            );
        }
    }

    void ProcessKeyboard(CameraMovement direction, float deltaTime) {
        float velocity = MovementSpeed * deltaTime;
        if (Mode == CameraMode::FIRST_PERSON) {
            if (direction == FORWARD)  Position += Front * velocity;
            if (direction == BACKWARD) Position -= Front * velocity;
            if (direction == LEFT)     Position -= Right * velocity;
            if (direction == RIGHT)    Position += Right * velocity;
            if (direction == UP)       Position += WorldUp * velocity;
            if (direction == DOWN)     Position -= WorldUp * velocity;
        } else {
            // Trong chế độ Orbit, W/S phóng to/thu nhỏ
            if (direction == FORWARD)  OrbitRadius = std::max(5.0f, OrbitRadius - velocity * 1.5f);
            if (direction == BACKWARD) OrbitRadius = std::min(80.0f, OrbitRadius + velocity * 1.5f);
            if (direction == LEFT)     OrbitAzimuth -= velocity * 8.0f;
            if (direction == RIGHT)    OrbitAzimuth += velocity * 8.0f;
        }
    }

    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        if (Mode == CameraMode::FIRST_PERSON) {
            Yaw += xoffset;
            Pitch += yoffset;

            if (constrainPitch) {
                Pitch = std::clamp(Pitch, -89.0f, 89.0f);
            }
            updateCameraVectors();
        } else {
            OrbitAzimuth += xoffset;
            OrbitElevation += yoffset;
            OrbitElevation = std::clamp(OrbitElevation, 5.0f, 85.0f);
        }
    }

    void ProcessMouseScroll(float yoffset) {
        if (Mode == CameraMode::FIRST_PERSON) {
            Zoom -= yoffset;
            Zoom = std::clamp(Zoom, 15.0f, 75.0f);
        } else {
            OrbitRadius -= yoffset * 1.5f;
            OrbitRadius = std::clamp(OrbitRadius, 5.0f, 80.0f);
        }
    }

    void ToggleMode() {
        if (Mode == CameraMode::FIRST_PERSON) {
            Mode = CameraMode::ORBIT;
        } else {
            Mode = CameraMode::FIRST_PERSON;
            Position = GetEyePosition();
            updateCameraVectors();
        }
    }

private:
    void updateCameraVectors() {
        glm::vec3 front;
        front.x = std::cos(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        front.y = std::sin(glm::radians(Pitch));
        front.z = std::sin(glm::radians(Yaw)) * std::cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};

} // namespace modern

