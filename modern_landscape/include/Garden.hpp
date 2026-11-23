#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

namespace modern {

struct GardenVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @brief Class Garden: Vườn hoa Keukenhof uốn lượn đa sắc màu & Thảm hoa nghệ thuật
 * Tái hiện chính xác kiệt tác từ ảnh mẫu 4 (Image 4):
 * 1. Các dải hoa uốn lượn hình sin/Bézier mềm mại (Flowing Tulip Ribbons) trải dài trên thảm cỏ:
 *    - Dải Vàng rực rỡ mặt trời (Golden Yellow)
 *    - Dải Đỏ Ruby nhung thẫm (Crimson Red)
 *    - Dải Hồng sen quý phái (Magenta / Hot Pink)
 *    - Dải Tím hoa cà hoàng gia (Royal Violet Purple)
 *    - Dải Trắng ngọc tinh khôi (Ivory White)
 * 2. Hàng rào viền cây xanh cắt tỉa thấp (Manicured Boxwood Borders) uốn lượn bám sát từng dải hoa.
 * 3. Chậu hoa đá châu Âu cổ điển (European Stone Urns) đặt tại lối vào và hiên nhà.
 */
class Garden {
public:
    GLuint VAO{0}, VBO{0}, EBO{0};
    unsigned int indexCount{0};

    Garden() = default;

    void init() {
        std::vector<GardenVertex> vertices;
        std::vector<unsigned int> indices;

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
                for (int i = 0; i < 4; ++i) vertices.push_back({p[faces[f][i]], norms[f], col});
                indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
                indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
            }
        };

        // Bảng màu hoa rực rỡ từ ảnh mẫu 4
        const glm::vec3 yellowTulip(1.0f,  0.88f, 0.04f); // Vàng tươi rực nắng
        const glm::vec3 redTulip(0.92f, 0.08f, 0.12f);    // Đỏ Ruby nhung thẫm
        const glm::vec3 pinkTulip(0.96f, 0.25f, 0.65f);   // Hồng sen Magenta
        const glm::vec3 purpleTulip(0.68f, 0.15f, 0.88f); // Tím Violet kiêu sa
        const glm::vec3 whiteTulip(0.98f, 0.98f, 0.95f);  // Trắng ngọc tuyết
        const glm::vec3 hedgeGreen(0.12f, 0.40f, 0.14f);  // Viền cây xén xanh đậm
        const glm::vec3 stemGreen(0.20f, 0.55f, 0.18f);   // Cuống hoa

        auto addFlower = [&](float x, float y, float z, float sz, const glm::vec3& petalCol) {
            // Cuống hoa
            addBox(x, y + sz * 0.5f, z, 0.04f, sz * 1.0f, 0.04f, stemGreen);
            // Bông hoa tulip cách điệu nở rộ
            addBox(x, y + sz * 1.0f, z, sz * 1.4f, sz * 0.8f, sz * 1.4f, petalCol);
            addBox(x, y + sz * 1.25f, z, sz * 0.7f, sz * 0.4f, sz * 0.7f, glm::mix(petalCol, glm::vec3(1.0f), 0.25f));
        };

        // =====================================================================
        // 1. CÁC DẢI HOA UỐN LƯỢN KEUKENHOF NGHỆ THUẬT (MẪU 4)
        // Khu vực sân vườn bên phải đài phun nước (X từ 4.0m đến 18.0m, Z từ 11.0m đến 24.0m)
        // =====================================================================
        struct RibbonBand {
            float xOffset;
            glm::vec3 color;
        };

        std::vector<RibbonBand> ribbons = {
            { 0.0f, yellowTulip },
            { 1.3f, pinkTulip   },
            { 2.6f, redTulip    },
            { 3.9f, purpleTulip },
            { 5.2f, yellowTulip },
            { 6.5f, whiteTulip  },
            { 7.8f, redTulip    }
        };

        constexpr int curvePoints = 28;
        for (const auto& ribbon : ribbons) {
            float baseX = 5.2f + ribbon.xOffset;

            for (int i = 0; i < curvePoints; ++i) {
                float t = static_cast<float>(i) / (curvePoints - 1);
                float z = glm::mix(11.0f, 24.5f, t);

                // Phương trình sóng uốn lượn S-Curve mềm mại (chuẩn Mẫu 4)
                float wave = std::sin(t * 3.14159f * 2.2f) * 1.8f;
                float x = baseX + wave;

                // Cụm hoa tulip dày đặc 3 nhánh trên mỗi bước sóng
                addFlower(x - 0.25f, 0.08f, z, 0.22f, ribbon.color);
                addFlower(x + 0.25f, 0.08f, z, 0.24f, ribbon.color);
                addFlower(x,         0.08f, z + 0.22f, 0.23f, ribbon.color);
            }
        }

        // Hàng rào viền cây xanh cắt tỉa uốn lượn ôm trọn hai bên thảm hoa (Mẫu 4)
        for (int i = 0; i < curvePoints; ++i) {
            float t = static_cast<float>(i) / (curvePoints - 1);
            float z = glm::mix(11.0f, 24.5f, t);
            float wave = std::sin(t * 3.14159f * 2.2f) * 1.8f;

            // Viền trái thảm hoa
            float xLeft = 5.2f - 0.7f + wave;
            addBox(xLeft, 0.18f, z, 0.40f, 0.36f, 0.50f, hedgeGreen);

            // Viền phải thảm hoa
            float xRight = 5.2f + 8.5f + wave;
            addBox(xRight, 0.18f, z, 0.40f, 0.36f, 0.50f, hedgeGreen);
        }

        // =====================================================================
        // 2. CÁC LUỐNG HOA NÂNG VIỀN ĐÁ TRƯỚC SẢNH BIỆT THỰ
        // =====================================================================
        const glm::vec3 stoneBorder(0.92f, 0.90f, 0.88f);
        auto addRaisedBed = [&](float cx, float cy, float cz, float w, float d,
                               const glm::vec3& petalCol, int countX, int countZ) {
            addBox(cx, cy + 0.14f, cz, w, 0.28f, d, stoneBorder);
            addBox(cx, cy + 0.20f, cz, w - 0.30f, 0.16f, d - 0.30f, glm::vec3(0.24f, 0.18f, 0.12f));

            float stepX = (w - 0.6f) / std::max(countX - 1, 1);
            float stepZ = (d - 0.6f) / std::max(countZ - 1, 1);
            float startX = cx - (w - 0.6f) * 0.5f;
            float startZ = cz - (d - 0.6f) * 0.5f;

            for (int r = 0; r < countZ; ++r) {
                for (int c = 0; c < countX; ++c) {
                    float fx = startX + c * stepX;
                    float fz = startZ + r * stepZ;
                    addFlower(fx, cy + 0.24f, fz, 0.18f, petalCol);
                }
            }
        };

        // Luống hoa hồng đỏ bên trái hiên
        addRaisedBed(-4.2f, 0.10f, 7.5f, 4.2f, 2.0f, redTulip, 6, 3);
        // Luống hoa cúc vàng bên phải hiên
        addRaisedBed( 4.2f, 0.10f, 7.5f, 4.2f, 2.0f, yellowTulip, 6, 3);

        // =====================================================================
        // 3. CHẬU HOA ĐÁ LỚN CHÂU ÂU (LUXURY PLANTERS / URNS)
        // =====================================================================
        auto addUrn = [&](float x, float y, float z, const glm::vec3& flowerCol) {
            // Chân bệ đá trắng
            addBox(x, y + 0.30f, z, 0.65f, 0.60f, 0.65f, glm::vec3(0.92f, 0.90f, 0.88f));
            // Chậu hoa loe miệng
            addBox(x, y + 0.68f, z, 0.82f, 0.22f, 0.82f, glm::vec3(0.96f, 0.94f, 0.92f));
            // Cụm hoa xum xuê nở tràn miệng chậu
            addBox(x, y + 0.95f, z, 0.95f, 0.45f, 0.95f, flowerCol);
            addBox(x, y + 1.20f, z, 0.55f, 0.25f, 0.55f, yellowTulip);
        };

        // 2 Chậu hoa đón khách tại cổng biệt thự
        addUrn(-20.5f, 0.0f, 30.5f, redTulip);
        addUrn(-15.5f, 0.0f, 30.5f, pinkTulip);

        // 2 Chậu hoa tại sảnh chính biệt thự
        addUrn(-1.8f, 0.30f, 5.2f, pinkTulip);
        addUrn( 1.8f, 0.30f, 5.2f, redTulip);

        // Hàng rào cây xanh phân tách khu vực hồ bơi
        addBox(2.8f, 0.45f, 7.5f, 0.45f, 0.80f, 4.0f, hedgeGreen);
        addBox(8.5f, 0.45f, 7.5f, 0.45f, 0.80f, 4.0f, hedgeGreen);
        addBox(13.5f, 0.45f, 1.0f, 0.45f, 0.80f, 8.5f, hedgeGreen);

        indexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GardenVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GardenVertex), (void*)offsetof(GardenVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GardenVertex), (void*)offsetof(GardenVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GardenVertex), (void*)offsetof(GardenVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~Garden() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};

} // namespace modern
