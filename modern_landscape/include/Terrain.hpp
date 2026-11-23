#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include "PerlinNoise.hpp"

namespace modern {

struct TerrainVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @brief Class Terrain: Tạo bề mặt địa hình đồi cỏ tự nhiên bằng Perlin Noise.
 * 
 * Thuật toán làm phẳng khu vực biệt thự (Radial Villa Flattening):
 * - Điểm gốc (0,0,0) đặt biệt thự và hồ bơi cần nền đất phẳng phiu.
 * - Ta dùng hàm suy giảm mượt (smoothstep) theo bán kính:
 *   r = sqrt(x^2 + z^2)
 *   weight = smoothstep(r_flat, r_blend, r)
 *   height(x, z) = raw_perlin_noise(x, z) * weight
 * - Giúp sân biệt thự bằng phẳng tuyệt đối, trong khi khu vườn ngoài nhấp nhô đồi cỏ tự nhiên.
 */
class Terrain {
public:
    GLuint VAO{0}, VBO{0}, EBO{0};
    unsigned int indexCount{0};
    PerlinNoise noiseGen{2026};

    // Kích thước thế giới
    float width{90.0f};
    float depth{90.0f};
    int resolutionX{120};
    int resolutionZ{120};

    // Bán kính làm phẳng khu khuôn viên biệt thự bên trong tường rào (26m)
    float flatRadius{25.8f};
    float blendRadius{38.0f};
    float maxHillHeight{3.6f};

    Terrain() = default;

    void init() {
        std::vector<TerrainVertex> vertices;
        std::vector<unsigned int> indices;

        vertices.reserve(resolutionX * resolutionZ);

        float dx = width / (resolutionX - 1);
        float dz = depth / (resolutionZ - 1);
        float startX = -width * 0.5f;
        float startZ = -depth * 0.5f;

        // 1. Tạo các đỉnh (Vertices)
        for (int i = 0; i < resolutionZ; ++i) {
            float z = startZ + i * dz;
            for (int j = 0; j < resolutionX; ++j) {
                float x = startX + j * dx;
                float y = getHeight(x, z);

                TerrainVertex v;
                v.position = glm::vec3(x, y, z);
                v.normal = calculateNormal(x, z);

                // Màu sắc thực vật sinh động (Lush grass gradient)
                // Càng lên đồi cao màu cỏ càng sáng đón nắng, ở dưới thấp màu xanh đậm đà
                float hRatio = glm::clamp((y + 0.5f) / maxHillHeight, 0.0f, 1.0f);
                glm::vec3 baseGreen(0.22f, 0.55f, 0.18f); // Cỏ xanh mướt
                glm::vec3 hillGreen(0.38f, 0.68f, 0.22f); // Cỏ non trên đồi
                glm::vec3 lawnGreen(0.28f, 0.62f, 0.25f); // Bãi cỏ sân biệt thự

                float r = std::sqrt(x * x + z * z);
                if (r < flatRadius) {
                    v.color = lawnGreen;
                } else {
                    v.color = glm::mix(baseGreen, hillGreen, hRatio);
                }

                vertices.push_back(v);
            }
        }

        // 2. Tạo chỉ số tam giác (Indices - Triangle Strip hoặc Grid Triangles)
        for (int i = 0; i < resolutionZ - 1; ++i) {
            for (int j = 0; j < resolutionX - 1; ++j) {
                unsigned int topLeft = i * resolutionX + j;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (i + 1) * resolutionX + j;
                unsigned int bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        indexCount = static_cast<unsigned int>(indices.size());

        // 3. Nạp dữ liệu vào Modern OpenGL Buffer (VAO/VBO/EBO)
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // aPos (location = 0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, position));
        glEnableVertexAttribArray(0);

        // aNormal (location = 1)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, normal));
        glEnableVertexAttribArray(1);

        // aColor (location = 2)
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)offsetof(TerrainVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    /**
     * @brief Tính độ cao địa hình tại tọa độ (x, z) kết hợp Perlin Noise và vùng phẳng biệt thự + mặt đường
     */
    float getHeight(float x, float z) const {
        // Vùng khuôn viên biệt thự và trục đường mặt tiền (X: -28m..28m, Z: -26m..44m)
        if (x >= -27.0f && x <= 27.0f && z >= -27.0f && z <= 44.0f) {
            return 0.0f; // Bằng phẳng tuyệt đối cho sân vườn, hồ bơi và lòng đường
        }

        // Tính khoảng cách tới hình chữ nhật phẳng
        float dx = std::max(0.0f, std::max(-27.0f - x, x - 27.0f));
        float dz = 0.0f;
        if (z < -27.0f) dz = -27.0f - z;
        else if (z > 44.0f) dz = z - 44.0f;

        float dist = std::sqrt(dx * dx + dz * dz);
        float blendDist = 12.0f; // Vùng chuyển tiếp 12m sang đồi núi tự nhiên

        if (dist <= 0.001f) return 0.0f;

        // Trọng số chuyển tiếp mượt (Smoothstep: 3t^2 - 2t^3)
        float t = glm::clamp(dist / blendDist, 0.0f, 1.0f);
        float weight = t * t * (3.0f - 2.0f * t);

        // Tần số noise đồi cỏ tự nhiên (0.04f = bước sóng ~25m)
        float rawNoise = noiseGen.fBm(x * 0.04f, z * 0.04f, 4, 0.45f, 2.1f);
        float microNoise = noiseGen.noise(x * 0.15f, z * 0.15f) * 0.25f;

        return (rawNoise * maxHillHeight + microNoise) * weight;
    }

    /**
     * @brief Tính toán pháp tuyến đỉnh (Normal Vector) bằng sai phân hữu hạn (Finite Difference)
     */
    glm::vec3 calculateNormal(float x, float z) const {
        constexpr float eps = 0.2f;
        float hL = getHeight(x - eps, z);
        float hR = getHeight(x + eps, z);
        float hD = getHeight(x, z - eps);
        float hU = getHeight(x, z + eps);

        // Gradient vector dH/dx và dH/dz
        glm::vec3 normal(hL - hR, 2.0f * eps, hD - hU);
        return glm::normalize(normal);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Terrain() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};

} // namespace modern

