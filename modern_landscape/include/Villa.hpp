#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace modern {

struct VillaVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @brief Class Villa: Khối kiến trúc biệt thự hiện đại tối giản (Modern Minimalist Villa)
 * Đặt tại gốc tọa độ (0,0,0) với các khối hình học vươn mái che công xôn (Cantilevered slabs),
 * mảng kính lớn suốt tầng, lam gỗ trang trí, cột thép đen thanh thoát và ban công kính.
 */
class Villa {
public:
    GLuint VAO{0}, VBO{0}, EBO{0};
    unsigned int indexCount{0};

    Villa() = default;

    void init() {
        std::vector<VillaVertex> vertices;
        std::vector<unsigned int> indices;

        // Bảng màu kiến trúc hiện đại (Modern Luxury Architecture Palette)
        const glm::vec3 whiteStucco(0.92f, 0.92f, 0.90f);  // Tường bê tông trắng tinh khôi
        const glm::vec3 darkConcrete(0.28f, 0.28f, 0.30f); // Bê tông xám than accent
        const glm::vec3 warmWood(0.72f, 0.44f, 0.22f);     // Lam gỗ ốp mặt tiền & trần hiên
        const glm::vec3 glassColor(0.25f, 0.65f, 0.78f);   // Kính cường lực xanh lơ phản quang
        const glm::vec3 blackMetal(0.15f, 0.15f, 0.16f);   // Cột thép & khung cửa nhôm đen
        const glm::vec3 terraceStone(0.78f, 0.76f, 0.72f); // Sàn ban công đá cẩm thạch

        auto addBox = [&](float x, float y, float z, float w, float h, float d, const glm::vec3& col) {
            float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;
            glm::vec3 p[8] = {
                {x - hw, y - hh, z - hd}, {x + hw, y - hh, z - hd},
                {x + hw, y + hh, z - hd}, {x - hw, y + hh, z - hd},
                {x - hw, y - hh, z + hd}, {x + hw, y - hh, z + hd},
                {x + hw, y + hh, z + hd}, {x - hw, y + hh, z + hd}
            };
            int faces[6][4] = {
                {0, 1, 2, 3}, {5, 4, 7, 6}, {4, 0, 3, 7},
                {1, 5, 6, 2}, {3, 2, 6, 7}, {4, 5, 1, 0}
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

        // 1. Đế móng sàn biệt thự (Terrace Foundation)
        addBox(0.0f, 0.15f, 0.0f, 15.0f, 0.30f, 14.0f, terraceStone);

        // 2. Khối phòng khách Tầng 1 (Ground Floor Living Pavilion)
        addBox(-1.5f, 1.85f, -1.0f, 9.0f, 3.10f, 8.0f, whiteStucco);

        // Vách kính cường lực lớn mặt tiền tầng 1
        addBox(-1.5f, 1.85f, 3.05f, 8.2f, 2.70f, 0.08f, glassColor);
        // Khung nhôm đen chia ô cửa kính
        for (int i = -3; i <= 3; ++i) {
            addBox(-1.5f + i * 1.35f, 1.85f, 3.07f, 0.08f, 2.80f, 0.12f, blackMetal);
        }

        // 3. Khối phòng Tầng 2 vươn cantilever (Upper Floor Cantilever Volume)
        addBox(0.5f, 4.75f, 0.0f, 11.5f, 2.80f, 9.0f, darkConcrete);

        // Ban công kính tầng 2
        addBox(1.5f, 4.65f, 4.55f, 7.5f, 2.40f, 0.08f, glassColor);
        addBox(1.5f, 3.85f, 4.90f, 8.2f, 0.90f, 0.06f, glassColor * 1.1f); // Lan can kính

        // 4. Mái bằng công xôn hiện đại (Flat Cantilevered Roof Slab)
        addBox(0.5f, 6.25f, 0.2f, 13.5f, 0.28f, 11.0f, whiteStucco);

        // 5. Lam gỗ chắn nắng mặt tiền (Vertical Wooden Louvers / Brise-Soleil)
        for (int i = 0; i < 9; ++i) {
            float louverX = -5.5f + i * 0.40f;
            addBox(louverX, 4.75f, 4.60f, 0.08f, 2.60f, 0.25f, warmWood);
        }

        // 6. Giàn che Pergola bằng gỗ hiện đại vươn ra sân vườn hồ bơi
        for (int i = 0; i < 6; ++i) {
            float pergolaZ = -1.5f + i * 1.0f;
            addBox(5.0f, 3.35f, pergolaZ, 4.5f, 0.12f, 0.12f, warmWood);
        }
        // Xà đỡ giàn pergola
        addBox(3.0f, 3.42f, 1.0f, 0.15f, 0.15f, 5.5f, blackMetal);
        addBox(7.0f, 3.42f, 1.0f, 0.15f, 0.15f, 5.5f, blackMetal);

        // 7. Cột thép chịu lực đen siêu thanh mảnh
        addBox(7.0f, 1.70f,  3.5f, 0.18f, 3.40f, 0.18f, blackMetal);
        addBox(7.0f, 1.70f, -1.5f, 0.18f, 3.40f, 0.18f, blackMetal);
        addBox(3.0f, 1.70f,  4.8f, 0.18f, 3.40f, 0.18f, blackMetal);

        // 8. Ống khói / Trụ ốp đá phong hóa hiện đại (Feature Fireplace Chimney)
        addBox(-4.8f, 3.60f, -2.5f, 1.4f, 7.20f, 1.4f, glm::vec3(0.42f, 0.40f, 0.38f));

        indexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VillaVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VillaVertex), (void*)offsetof(VillaVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VillaVertex), (void*)offsetof(VillaVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VillaVertex), (void*)offsetof(VillaVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Villa() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};

} // namespace modern

