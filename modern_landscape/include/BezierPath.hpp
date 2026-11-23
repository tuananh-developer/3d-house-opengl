#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include "Terrain.hpp"

namespace modern {

struct PathVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @brief Class Pathway: Lối đi lát đá uốn lượn bằng đường cong Bézier bậc 3 (Cubic Bézier Curve).
 * 
 * Công thức toán học Đường cong Bézier bậc 3:
 * B(t) = (1 - t)^3 * P0 + 3 * (1 - t)^2 * t * P1 + 3 * (1 - t) * t^2 * P2 + t^3 * P3, với t in [0, 1]
 * 
 * Vector tiếp tuyến (Đạo hàm bậc nhất B'(t)):
 * B'(t) = 3*(1-t)^2 * (P1 - P0) + 6*(1-t)*t * (P2 - P1) + 3*t^2 * (P3 - P2)
 * 
 * Vector pháp tuyến ngang mặt phẳng (Frenet Frame tangent / normal):
 * Tangent T(t) = normalize(B'(t))
 * Normal N(t) = normalize(cross(T(t), vec3(0, 1, 0)))
 * 
 * Dựa vào N(t), ta dựng các phiến đá lát lối đi hình hộp chữ nhật có độ dày,
 * bề rộng w, khoảng cách khe rãnh tự nhiên, và bám sát mặt đồi cỏ bằng cách
 * lấy độ cao Y từ hàm Terrain::getHeight(x, z) + offset.
 */
class Pathway {
public:
    GLuint VAO{0}, VBO{0}, EBO{0};
    unsigned int indexCount{0};

    // 4 Điểm kiểm soát (Control Points) định hình đường cong chữ S từ cổng đến hiên biệt thự
    glm::vec3 P0{-18.0f, 0.0f, 32.0f}; // Điểm đầu tại cổng biệt thự
    glm::vec3 P1{-14.0f, 0.0f, 23.0f}; // Điểm uốn thứ nhất lượn qua khóm hoa và cây bóng mát
    glm::vec3 P2{-10.5f, 0.0f, 11.0f}; // Điểm uốn thứ hai lượn sát quảng trường đài phun nước
    glm::vec3 P3{  0.0f, 0.0f,  6.0f}; // Điểm cuối tại sảnh chính biệt thự

    float pathWidth{1.6f};
    int numSlabs{48}; // Số lượng phiến đá lát dọc đường cong

    Pathway() = default;

    // Tính điểm trên đường cong Bézier bậc 3 tại tham số t in [0, 1]
    glm::vec3 evaluateBezier(float t) const {
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;

        return uuu * P0 + 3.0f * uu * t * P1 + 3.0f * u * tt * P2 + ttt * P3;
    }

    // Tính vector tiếp tuyến B'(t)
    glm::vec3 evaluateTangent(float t) const {
        float u = 1.0f - t;
        glm::vec3 d = 3.0f * u * u * (P1 - P0) +
                      6.0f * u * t * (P2 - P1) +
                      3.0f * t * t * (P3 - P2);
        if (glm::length(d) < 0.0001f) return glm::vec3(0.0f, 0.0f, -1.0f);
        return glm::normalize(d);
    }

    void init(const Terrain& terrain) {
        std::vector<PathVertex> vertices;
        std::vector<unsigned int> indices;

        // Màu đá phiến tự nhiên (Natural Travertine / Slate stone)
        const glm::vec3 stoneTopColor(0.82f, 0.80f, 0.74f);  // Mặt trên đá sáng màu sang trọng
        const glm::vec3 stoneSideColor(0.55f, 0.52f, 0.48f); // Cạnh đá xám trầm
        const glm::vec3 curbColor(0.35f, 0.34f, 0.32f);      // Viền đá bó vỉa

        const float slabThickness = 0.08f;
        const float halfWidth = pathWidth * 0.5f;

        for (int i = 0; i < numSlabs; ++i) {
            float t0 = static_cast<float>(i) / numSlabs;
            float t1 = static_cast<float>(i + 1) / numSlabs;

            // Để lại khe hở nhỏ giữa các phiến đá (mortar gap)
            float t0_pad = t0 + 0.003f;
            float t1_pad = t1 - 0.003f;

            glm::vec3 center0 = evaluateBezier(t0_pad);
            glm::vec3 center1 = evaluateBezier(t1_pad);

            glm::vec3 tan0 = evaluateTangent(t0_pad);
            glm::vec3 tan1 = evaluateTangent(t1_pad);

            glm::vec3 norm0 = glm::normalize(glm::cross(tan0, glm::vec3(0.0f, 1.0f, 0.0f)));
            glm::vec3 norm1 = glm::normalize(glm::cross(tan1, glm::vec3(0.0f, 1.0f, 0.0f)));

            // 4 Góc của mặt trên phiến đá (Top Face)
            glm::vec3 p0_top = center0 - norm0 * halfWidth;
            glm::vec3 p1_top = center0 + norm0 * halfWidth;
            glm::vec3 p2_top = center1 + norm1 * halfWidth;
            glm::vec3 p3_top = center1 - norm1 * halfWidth;

            // Bám theo độ cao địa hình Perlin Noise tại từng góc (+ offset 0.05m chống z-fighting)
            p0_top.y = terrain.getHeight(p0_top.x, p0_top.z) + slabThickness;
            p1_top.y = terrain.getHeight(p1_top.x, p1_top.z) + slabThickness;
            p2_top.y = terrain.getHeight(p2_top.x, p2_top.z) + slabThickness;
            p3_top.y = terrain.getHeight(p3_top.x, p3_top.z) + slabThickness;

            // 4 Góc của mặt dưới phiến đá (Bottom Face cắm xuống đất)
            glm::vec3 p0_bot = p0_top - glm::vec3(0.0f, slabThickness * 1.5f, 0.0f);
            glm::vec3 p1_bot = p1_top - glm::vec3(0.0f, slabThickness * 1.5f, 0.0f);
            glm::vec3 p2_bot = p2_top - glm::vec3(0.0f, slabThickness * 1.5f, 0.0f);
            glm::vec3 p3_bot = p3_top - glm::vec3(0.0f, slabThickness * 1.5f, 0.0f);

            // Vector pháp tuyến mặt trên
            glm::vec3 topNormal = glm::normalize(glm::cross(p1_top - p0_top, p3_top - p0_top));

            auto addQuad = [&](const glm::vec3& a, const glm::vec3& b,
                               const glm::vec3& c, const glm::vec3& d,
                               const glm::vec3& n, const glm::vec3& col) {
                unsigned int baseIdx = static_cast<unsigned int>(vertices.size());
                vertices.push_back({a, n, col});
                vertices.push_back({b, n, col});
                vertices.push_back({c, n, col});
                vertices.push_back({d, n, col});

                indices.push_back(baseIdx + 0);
                indices.push_back(baseIdx + 1);
                indices.push_back(baseIdx + 2);

                indices.push_back(baseIdx + 0);
                indices.push_back(baseIdx + 2);
                indices.push_back(baseIdx + 3);
            };

            // Mặt trên phiến đá
            addQuad(p0_top, p1_top, p2_top, p3_top, topNormal, stoneTopColor);

            // Cạnh trái
            glm::vec3 leftNorm = -norm0;
            addQuad(p3_top, p0_top, p0_bot, p3_bot, leftNorm, stoneSideColor);

            // Cạnh phải
            glm::vec3 rightNorm = norm0;
            addQuad(p1_top, p2_top, p2_bot, p1_bot, rightNorm, stoneSideColor);

            // Cạnh trước & sau
            addQuad(p0_top, p1_top, p1_bot, p0_bot, -tan0, stoneSideColor);
            addQuad(p2_top, p3_top, p3_bot, p2_bot, tan1, stoneSideColor);

            // Đèn lồng năng lượng mặt trời cắm đất mini dọc lối đi (Solar Path Bollards)
            if (i % 6 == 0) {
                // Đèn bên trái
                glm::vec3 lightPosL = p0_top - norm0 * 0.25f;
                glm::vec3 postCol(0.2f, 0.2f, 0.22f);
                glm::vec3 glassCol(0.98f, 0.85f, 0.35f);
                addQuad(lightPosL + glm::vec3(-0.04f, 0.0f, -0.04f),
                        lightPosL + glm::vec3( 0.04f, 0.0f, -0.04f),
                        lightPosL + glm::vec3( 0.04f, 0.35f, -0.04f),
                        lightPosL + glm::vec3(-0.04f, 0.35f, -0.04f),
                        glm::vec3(0, 0, -1), postCol);
                addQuad(lightPosL + glm::vec3(-0.06f, 0.35f, -0.06f),
                        lightPosL + glm::vec3( 0.06f, 0.35f, -0.06f),
                        lightPosL + glm::vec3( 0.06f, 0.45f, -0.06f),
                        lightPosL + glm::vec3(-0.06f, 0.45f, -0.06f),
                        glm::vec3(0, 0, -1), glassCol);
            }
        }

        indexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PathVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PathVertex), (void*)offsetof(PathVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PathVertex), (void*)offsetof(PathVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(PathVertex), (void*)offsetof(PathVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Pathway() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};

} // namespace modern

