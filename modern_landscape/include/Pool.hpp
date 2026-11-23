#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace modern {

struct PoolVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @brief Class Pool: Hồ bơi vô cực hiện đại (Modern Infinity Pool)
 * Thiết kế sunken chìm tinh tế với sàn gỗ teak, thành đá travertine trắng,
 * lòng hồ lát gạch mosaic xanh ngọc và mặt nước phản chiếu ánh sáng lấp lánh.
 */
class Pool {
public:
    GLuint basinVAO{0}, basinVBO{0}, basinEBO{0};
    unsigned int basinIndexCount{0};

    GLuint waterVAO{0}, waterVBO{0}, waterEBO{0};
    unsigned int waterIndexCount{0};

    // Tọa độ và kích thước hồ bơi
    float minX{3.5f}, maxX{11.5f};
    float minZ{-2.5f}, maxZ{4.5f};
    float deckY{0.08f};
    float waterY{-0.15f};
    float floorY{-1.40f};

    Pool() = default;

    void init() {
        initBasin();
        initWaterMesh();
    }

    void initBasin() {
        std::vector<PoolVertex> vertices;
        std::vector<unsigned int> indices;

        const glm::vec3 deckWood(0.68f, 0.42f, 0.22f);      // Gỗ Teak decking quanh hồ
        const glm::vec3 stoneBorder(0.88f, 0.86f, 0.82f);  // Viền đá Travertine trắng ngà
        const glm::vec3 mosaicTile(0.08f, 0.55f, 0.72f);   // Gạch mosaic xanh lam ngọc lòng hồ
        const glm::vec3 loungeChair(0.92f, 0.90f, 0.85f);  // Ghế tắm nắng hồ bơi trắng

        auto addBox = [&](float x, float y, float z, float w, float h, float d, const glm::vec3& col) {
            float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
            glm::vec3 p[8] = {
                {x - hw, y - hh, z - hd}, {x + hw, y - hh, z - hd},
                {x + hw, y + hh, z - hd}, {x - hw, y + hh, z - hd},
                {x - hw, y - hh, z + hd}, {x + hw, y - hh, z + hd},
                {x + hw, y + hh, z + hd}, {x - hw, y + hh, z + hd}
            };
            int faces[6][4] = {
                {0, 1, 2, 3}, // Front (-Z)
                {5, 4, 7, 6}, // Back (+Z)
                {4, 0, 3, 7}, // Left (-X)
                {1, 5, 6, 2}, // Right (+X)
                {3, 2, 6, 7}, // Top (+Y)
                {4, 5, 1, 0}  // Bottom (-Y)
            };
            glm::vec3 norms[6] = {
                {0, 0, -1}, {0, 0, 1}, {-1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, -1, 0}
            };

            for (int f = 0; f < 6; ++f) {
                unsigned int base = static_cast<unsigned int>(vertices.size());
                for (int i = 0; i < 4; ++i) {
                    vertices.push_back({p[faces[f][i]], norms[f], col});
                }
                indices.push_back(base + 0);
                indices.push_back(base + 1);
                indices.push_back(base + 2);
                indices.push_back(base + 0);
                indices.push_back(base + 2);
                indices.push_back(base + 3);
            }
        };

        // 1. Sàn gỗ tắm nắng Teak Wood bao quanh hồ bơi
        addBox((minX + maxX) * 0.5f + 1.0f, deckY * 0.5f, (minZ + maxZ) * 0.5f,
               (maxX - minX) + 3.6f, deckY, (maxZ - minZ) + 3.2f, deckWood);

        // 2. Viền đá cẩm thạch Travertine bao quanh miệng hồ bơi
        float borderThick = 0.35f;
        // Viền Bắc (-Z)
        addBox((minX + maxX) * 0.5f, deckY + 0.02f, minZ - borderThick * 0.5f,
               (maxX - minX) + borderThick * 2.0f, 0.04f, borderThick, stoneBorder);
        // Viền Nam (+Z)
        addBox((minX + maxX) * 0.5f, deckY + 0.02f, maxZ + borderThick * 0.5f,
               (maxX - minX) + borderThick * 2.0f, 0.04f, borderThick, stoneBorder);
        // Viền Tây (-X)
        addBox(minX - borderThick * 0.5f, deckY + 0.02f, (minZ + maxZ) * 0.5f,
               borderThick, 0.04f, (maxZ - minZ), stoneBorder);
        // Viền Đông (+X) - Cạnh tràn vô cực Infinity Edge
        addBox(maxX + borderThick * 0.5f, deckY - 0.04f, (minZ + maxZ) * 0.5f,
               borderThick, 0.04f, (maxZ - minZ), stoneBorder);

        // 3. Đáy hồ bơi lát gạch mosaic xanh ngọc
        float poolW = maxX - minX;
        float poolD = maxZ - minZ;
        float poolH = deckY - floorY;
        addBox((minX + maxX) * 0.5f, floorY, (minZ + maxZ) * 0.5f, poolW, 0.08f, poolD, mosaicTile);

        // 4. Các vách thành hồ chìm
        // Thành Tây (-X)
        addBox(minX, floorY + poolH * 0.5f, (minZ + maxZ) * 0.5f, 0.12f, poolH, poolD, mosaicTile);
        // Thành Bắc (-Z)
        addBox((minX + maxX) * 0.5f, floorY + poolH * 0.5f, minZ, poolW, poolH, 0.12f, mosaicTile);
        // Thành Nam (+Z)
        addBox((minX + maxX) * 0.5f, floorY + poolH * 0.5f, maxZ, poolW, poolH, 0.12f, mosaicTile);

        // 5. Bậc thang chìm xuống hồ (Submerged Steps)
        addBox(minX + 0.6f, floorY + 0.35f, minZ + 1.2f, 1.2f, 0.70f, 1.6f, mosaicTile * 1.1f);
        addBox(minX + 1.3f, floorY + 0.18f, minZ + 1.2f, 0.8f, 0.36f, 1.6f, mosaicTile * 1.15f);

        // 6. Ghế tắm nắng hồ bơi phong cách nghỉ dưỡng (Sun Loungers & Umbrella)
        for (int i = 0; i < 2; ++i) {
            float chairZ = minZ + 1.2f + i * 2.4f;
            float chairX = maxX + 1.8f;
            // Khung ghế
            addBox(chairX, deckY + 0.16f, chairZ, 1.8f, 0.12f, 0.70f, loungeChair);
            // Gối tựa đầu nâng cao
            addBox(chairX + 0.65f, deckY + 0.28f, chairZ, 0.50f, 0.15f, 0.70f, glm::vec3(0.3f, 0.7f, 0.85f));
        }

        basinIndexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &basinVAO);
        glGenBuffers(1, &basinVBO);
        glGenBuffers(1, &basinEBO);

        glBindVertexArray(basinVAO);
        glBindBuffer(GL_ARRAY_BUFFER, basinVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PoolVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, basinEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PoolVertex), (void*)offsetof(PoolVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PoolVertex), (void*)offsetof(PoolVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(PoolVertex), (void*)offsetof(PoolVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void initWaterMesh() {
        std::vector<PoolVertex> vertices;
        std::vector<unsigned int> indices;

        // Lưới bề mặt nước chi tiết (32 x 24 quads) để shader làm gợn sóng
        constexpr int resX = 32;
        constexpr int resZ = 24;

        float dx = (maxX - minX) / (resX - 1);
        float dz = (maxZ - minZ) / (resZ - 1);

        glm::vec3 waterColor(0.12f, 0.75f, 0.92f); // Nước trong vắt xanh ngọc lam

        for (int i = 0; i < resZ; ++i) {
            float z = minZ + i * dz;
            for (int j = 0; j < resX; ++j) {
                float x = minX + j * dx;
                vertices.push_back({{x, waterY, z}, {0.0f, 1.0f, 0.0f}, waterColor});
            }
        }

        for (int i = 0; i < resZ - 1; ++i) {
            for (int j = 0; j < resX - 1; ++j) {
                unsigned int topLeft = i * resX + j;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (i + 1) * resX + j;
                unsigned int bottomRight = bottomLeft + 1;

                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        waterIndexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &waterVAO);
        glGenBuffers(1, &waterVBO);
        glGenBuffers(1, &waterEBO);

        glBindVertexArray(waterVAO);
        glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PoolVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PoolVertex), (void*)offsetof(PoolVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PoolVertex), (void*)offsetof(PoolVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(PoolVertex), (void*)offsetof(PoolVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void drawBasin() const {
        glBindVertexArray(basinVAO);
        glDrawElements(GL_TRIANGLES, basinIndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    void drawWater() const {
        glBindVertexArray(waterVAO);
        glDrawElements(GL_TRIANGLES, waterIndexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Pool() {
        if (basinVAO) glDeleteVertexArrays(1, &basinVAO);
        if (basinVBO) glDeleteBuffers(1, &basinVBO);
        if (basinEBO) glDeleteBuffers(1, &basinEBO);
        if (waterVAO) glDeleteVertexArrays(1, &waterVAO);
        if (waterVBO) glDeleteBuffers(1, &waterVBO);
        if (waterEBO) glDeleteBuffers(1, &waterEBO);
    }
};

} // namespace modern

