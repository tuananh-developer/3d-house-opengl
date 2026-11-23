#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace modern {

/**
 * @brief Class ShadowMap: Khởi tạo Framebuffer Object (FBO) và Depth Texture
 * để tính toán đổ bóng thời gian thực (Real-time Shadow Mapping).
 * 
 * Nguyên lý Thuật toán Shadow Mapping:
 * - Pass 1 (Render từ góc nhìn nguồn sáng mặt trời):
 *   + Nguồn sáng mặt trời chiếu xiên (Directional Light) dùng phép chiếu trực giao (Orthographic Projection).
 *   + Ma trận LightSpaceMatrix = lightProjection * lightView.
 *   + Vẽ toàn bộ cảnh vào Depth Buffer của FBO để ghi lại khoảng cách z gần nhất từ nguồn sáng (Depth Map).
 * - Pass 2 (Render từ góc nhìn Camera chính):
 *   + Gửi ma trận LightSpaceMatrix và Depth Texture sang GLSL Fragment Shader.
 *   + Shader chuyển đổi tọa độ mảnh (FragPos) sang không gian ánh sáng (Light Space coordinates).
 *   + So sánh độ sâu: Nếu fragDepth > shadowMapDepth + bias -> Điểm nằm trong vùng bóng tối (Shadow = 1.0).
 *   + Kỹ thuật PCF (Percentage-Closer Filtering 3x3): Lấy mẫu trung bình 9 texel lân cận để làm mịn
 *     mép bóng râm (Soft Shadows), không bị răng cưa pixel.
 */
class ShadowMap {
public:
    GLuint depthFBO{0};
    GLuint depthMap{0};
    unsigned int resolution{2048}; // Độ phân giải 2K sắc nét

    glm::vec3 lightPos{-22.0f, 32.0f, 18.0f}; // Vị trí mặt trời chiếu xiên
    glm::vec3 lightTarget{0.0f, 0.0f, 0.0f};
    glm::vec3 lightColor{1.0f, 0.96f, 0.88f};  // Ánh nắng vàng ấm tự nhiên

    float orthoSize{45.0f};
    float nearPlane{1.0f};
    float farPlane{85.0f};

    ShadowMap() = default;

    bool init(unsigned int res = 2048) {
        resolution = res;

        glGenFramebuffers(1, &depthFBO);

        // Tạo Texture lưu trữ độ sâu 32-bit float
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                     resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        // Đặt màu biên là 1.0 (xa nhất) để các vùng ngoài tầm frustum không bị đen bóng râm
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

        // Shadow Map chỉ cần Depth, không cần Color buffer
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR::SHADOWMAP::Framebuffer is not complete!" << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    glm::mat4 getLightSpaceMatrix() const {
        glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);
        glm::mat4 lightView = glm::lookAt(lightPos, lightTarget, glm::vec3(0.0f, 1.0f, 0.0f));
        return lightProjection * lightView;
    }

    glm::vec3 getLightDir() const {
        return glm::normalize(lightTarget - lightPos);
    }

    void beginDepthPass() const {
        glViewport(0, 0, resolution, resolution);
        glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT); // Khử Peter Panning (shadow acne)
    }

    void endDepthPass(int screenWidth, int screenHeight) const {
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, screenWidth, screenHeight);
    }

    void bindDepthTexture(GLenum textureUnit = GL_TEXTURE0) const {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, depthMap);
    }

    ~ShadowMap() {
        if (depthFBO) glDeleteFramebuffers(1, &depthFBO);
        if (depthMap) glDeleteTextures(1, &depthMap);
    }
};

} // namespace modern

