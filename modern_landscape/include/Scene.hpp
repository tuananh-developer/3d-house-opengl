#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include "Shader.hpp"
#include "Camera.hpp"
#include "Terrain.hpp"
#include "BezierPath.hpp"
#include "ProceduralTree.hpp"
#include "Pool.hpp"
#include "Villa.hpp"
#include "Fountain.hpp"
#include "Garden.hpp"
#include "ModernWalls.hpp"
#include "ShadowMap.hpp"

namespace modern {

// GLSL Shaders nhúng trực tiếp (Inline source) để ứng dụng tự hành 100% không sợ mất file
namespace Shaders {

inline const char* depthVertex = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 lightSpaceMatrix;
uniform mat4 model;

void main() {
    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);
}
)";

inline const char* depthFragment = R"(
#version 330 core
void main() {
    // Tự động ghi gl_FragDepth
}
)";

inline const char* mainVertex = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec3 VertexColor;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    VertexColor = aColor;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

inline const char* mainFragment = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;
in vec4 FragPosLightSpace;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D shadowMap;
uniform float shininess;
uniform float specularStrength;

// Thuật toán Shadow Mapping kết hợp bộ lọc PCF (Percentage-Closer Filtering 3x3)
float CalculateShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir) {
    // Chuyển đổi từ clip space sang normalized device coordinates [-1, 1]
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Ánh xạ sang khoảng tọa độ texture [0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    // Nếu mảnh nằm ngoài frustum trực giao của nguồn sáng thì không đổ bóng
    if (projCoords.z > 1.0) return 0.0;

    // Slope-scale depth bias: bề mặt nghiêng cần bias lớn hơn để khử sọc bóng râm (shadow acne)
    float bias = max(0.0035 * (1.0 - dot(normal, lightDir)), 0.0006);

    // PCF 3x3 làm mịn đường viền bóng đổ
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth) ? 1.0 : 0.0;
        }
    }
    return shadow / 9.0;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDirection = normalize(viewPos - FragPos);

    // 1. Thành phần Ánh sáng môi trường (Ambient)
    vec3 ambient = 0.38 * VertexColor;

    // 2. Thành phần Tán xạ (Lambertian Diffuse)
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * VertexColor;

    // 3. Thành phần Khúc xạ phản chiếu (Blinn-Phong Specular với Halfway Vector H)
    vec3 halfwayDir = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // 4. Tính toán bóng râm thời gian thực
    float shadow = CalculateShadow(FragPosLightSpace, norm, lightDirection);
    vec3 lighting = ambient + (1.0 - shadow * 0.72) * (diffuse + specular);

    FragColor = vec4(lighting, 1.0);
}
)";

inline const char* waterVertex = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform float time;

void main() {
    vec3 pos = aPos;
    // Phương trình sóng gợn nước thời gian thực (Gerstner / Sinusoidal Wave superposition)
    float wave1 = 0.025 * sin(pos.x * 4.5 + time * 3.2);
    float wave2 = 0.018 * cos(pos.z * 5.2 + time * 2.8);
    float wave3 = 0.012 * sin((pos.x + pos.z) * 3.0 + time * 4.0);
    pos.y += (wave1 + wave2 + wave3);

    FragPos = vec3(model * vec4(pos, 1.0));

    // Vector pháp tuyến gợn sóng biến thiên
    vec3 perturbedNorm = normalize(vec3(
        -0.12 * cos(pos.x * 4.5 + time * 3.2) - 0.04 * cos((pos.x + pos.z) * 3.0 + time * 4.0),
        1.0,
        0.10 * sin(pos.z * 5.2 + time * 2.8) - 0.04 * cos((pos.x + pos.z) * 3.0 + time * 4.0)
    ));

    Normal = mat3(transpose(inverse(model))) * perturbedNorm;
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

inline const char* waterFragment = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform sampler2D shadowMap;

void main() {
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-lightDir);
    vec3 viewDirection = normalize(viewPos - FragPos);

    // Màu nước hồ bơi trong vắt nhiệt đới
    vec3 waterShallow = vec3(0.15, 0.78, 0.95);
    vec3 waterDeep    = vec3(0.06, 0.45, 0.70);

    // Hệ số Fresnel: nhìn góc xiên thì phản xạ nhiều hơn, nhìn từ trên xuống thì trong suốt
    float fresnel = pow(1.0 - max(dot(norm, viewDirection), 0.0), 3.0);
    vec3 baseColor = mix(waterDeep, waterShallow, fresnel * 0.5 + 0.5);

    vec3 ambient = 0.45 * baseColor;
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;

    // Blinn-Phong Specular cực cao tạo tia nắng lấp lánh trên sóng nước
    vec3 halfwayDir = normalize(lightDirection + viewDirection);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 160.0);
    vec3 specular = 1.6 * spec * vec3(1.0, 1.0, 0.95);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 0.88);
}
)";

} // namespace Shaders

/**
 * @brief Class Scene: Quản lý toàn bộ vòng đời đối tượng ngoại cảnh và 2 Render Passes (Shadow Map + Shading)
 */
class Scene {
public:
    Terrain terrain;
    Pathway pathway;
    ProceduralTreeManager trees;
    Pool pool;
    Villa villa;
    Fountain fountain;
    Garden garden;
    ModernWalls walls;
    ShadowMap shadowMap;

    std::unique_ptr<Shader> depthShader;
    std::unique_ptr<Shader> mainShader;
    std::unique_ptr<Shader> waterShader;

    float elapsedTime{0.0f};

    Scene() = default;

    void init(int /*screenWidth*/, int /*screenHeight*/) {
        // 1. Biên dịch Shaders
        depthShader = std::make_unique<Shader>(Shaders::depthVertex, Shaders::depthFragment);
        mainShader  = std::make_unique<Shader>(Shaders::mainVertex,  Shaders::mainFragment);
        waterShader = std::make_unique<Shader>(Shaders::waterVertex, Shaders::waterFragment);

        // 2. Khởi tạo FBO Shadow Mapping
        shadowMap.init(2048);

        // 3. Khởi tạo đối tượng hình học
        terrain.init();
        pathway.init(terrain);
        trees.init();
        pool.init();
        villa.init();
        fountain.init();
        garden.init();
        walls.init();

        // Cấu hình OpenGL Core Profile
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
    }

    void update(float deltaTime) {
        elapsedTime += deltaTime;
    }

    void render(const Camera& camera, int screenWidth, int screenHeight) {
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 lightSpaceMatrix = shadowMap.getLightSpaceMatrix();

        // =====================================================================
        // PASS 1: RENDER SHADOW DEPTH MAP TỪ GÓC NHÌN MẶT TRỜI
        // =====================================================================
        shadowMap.beginDepthPass();
        depthShader->use();
        depthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        depthShader->setMat4("model", model);

        // Vẽ các vật thể tạo bóng (Casting shadows)
        villa.draw();
        walls.draw();
        fountain.drawBasin();
        garden.draw();
        trees.draw();
        pathway.draw();
        pool.drawBasin();
        terrain.draw();

        shadowMap.endDepthPass(screenWidth, screenHeight);

        // =====================================================================
        // PASS 2: RENDER SCENE CHÍNH VỚI BLINN-PHONG + SHADOW MAPPING
        // =====================================================================
        // Bầu trời xanh nhạt thanh bình (Clear Color Sky)
        glClearColor(0.55f, 0.76f, 0.92f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                                static_cast<float>(screenWidth) / static_cast<float>(std::max(screenHeight, 1)),
                                                0.1f, 180.0f);
        glm::mat4 view = camera.GetViewMatrix();

        // 1. Kích hoạt Main Shader
        mainShader->use();
        mainShader->setMat4("projection", projection);
        mainShader->setMat4("view", view);
        mainShader->setMat4("model", model);
        mainShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);

        mainShader->setVec3("lightDir", shadowMap.getLightDir());
        mainShader->setVec3("lightColor", shadowMap.lightColor);
        mainShader->setVec3("viewPos", camera.GetEyePosition());

        // Gán Texture Shadow Map vào Texture Unit 0
        shadowMap.bindDepthTexture(GL_TEXTURE0);
        mainShader->setInt("shadowMap", 0);

        // Thiết lập tham số Blinn-Phong cho Địa hình cỏ (nhám mờ)
        mainShader->setFloat("shininess", 16.0f);
        mainShader->setFloat("specularStrength", 0.12f);
        terrain.draw();

        // Tường rào kiến trúc hiện đại & Cổng biệt thự luxury
        mainShader->setFloat("shininess", 32.0f);
        mainShader->setFloat("specularStrength", 0.25f);
        walls.draw();

        // Đường lát đá Bézier (nhẵn vừa phải)
        mainShader->setFloat("shininess", 32.0f);
        mainShader->setFloat("specularStrength", 0.28f);
        pathway.draw();

        // Cây xanh & Bụi cây L-System đa dạng (Cọ nhiệt đới, Tùng tháp, Bonsai)
        mainShader->setFloat("shininess", 24.0f);
        mainShader->setFloat("specularStrength", 0.20f);
        trees.draw();

        // Thành hồ bơi & Sàn gỗ
        mainShader->setFloat("shininess", 48.0f);
        mainShader->setFloat("specularStrength", 0.40f);
        pool.drawBasin();

        // Biệt thự hiện đại (Tường trắng, kính, kim loại)
        mainShader->setFloat("shininess", 64.0f);
        mainShader->setFloat("specularStrength", 0.55f);
        villa.draw();

        // Tháp phun nước nghệ thuật (Đá cẩm thạch trắng Carrara & Viền đồng thau)
        mainShader->setFloat("shininess", 64.0f);
        mainShader->setFloat("specularStrength", 0.45f);
        fountain.drawBasin();

        // Vườn hoa rực rỡ & Thảm cây cắt tỉa tinh xảo
        mainShader->setFloat("shininess", 20.0f);
        mainShader->setFloat("specularStrength", 0.18f);
        garden.draw();

        // 2. Kích hoạt Water Shader cho mặt nước hồ bơi và đài phun nước
        glDisable(GL_CULL_FACE); // Mặt nước vẽ 2 chiều
        waterShader->use();
        waterShader->setMat4("projection", projection);
        waterShader->setMat4("view", view);
        waterShader->setMat4("model", model);
        waterShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
        waterShader->setVec3("lightDir", shadowMap.getLightDir());
        waterShader->setVec3("lightColor", shadowMap.lightColor);
        waterShader->setVec3("viewPos", camera.GetEyePosition());
        waterShader->setFloat("time", elapsedTime);

        shadowMap.bindDepthTexture(GL_TEXTURE0);
        waterShader->setInt("shadowMap", 0);

        pool.drawWater();
        fountain.drawWater();
        glEnable(GL_CULL_FACE);
        glEnable(GL_CULL_FACE);
    }
};

} // namespace modern
