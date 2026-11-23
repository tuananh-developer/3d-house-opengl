#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>

namespace modern {

struct TreeVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @brief Class ProceduralTreeManager: Hệ thống cây cảnh quan tự nhiên & kiến trúc nghỉ dưỡng
 * 
 * Tái hiện xuất sắc các mẫu cây thực tế từ ảnh minh họa:
 * 1. CÂY BÓNG MÁT TỰ NHIÊN HỮU CƠ (Organic Stylized Shade Tree - CHUẨN MẪU 3):
 *    - Gốc rễ xòe rộng ôm mặt đất (Flaring buttress roots).
 *    - Thân gỗ uốn khúc tự nhiên, phân nhánh thành 3-4 cành chính.
 *    - Tán lá dạng các khối mây tròn bồng bềnh, xum xuê (Voluminous puffy foliage clusters),
 *      kết cấu phân tầng tạo bóng đổ mềm mại, không bị rời rạc đơn điệu.
 * 2. CÂY CỌ NGHỈ DƯỠNG (Tropical Resort Palms - CHUẨN MẪU 1):
 *    - Thân cao thanh mảnh vươn cong hướng hồ bơi, tán lá cọ nan quạt buông rủ nhiệt đới.
 * 3. TÙNG THÁP ĐỊA TRUNG HẢI (Mediterranean Pencil Cypress - CHUẨN MẪU 1):
 *    - Dáng vút cao xanh ngắt quý phái viền dọc theo tường rào và ranh giới biệt thự.
 * 4. BỤI CÂY TRÒN XÉN TỈA TINH XẢO (Boxwood Topiary Spheres) đặt dọc bậc thềm.
 * 5. RỪNG CÂY TỰ NHIÊN NỀN (Background Forest) trên các đồi Perlin bao quanh.
 */
class ProceduralTreeManager {
public:
    GLuint VAO{0}, VBO{0}, EBO{0};
    unsigned int indexCount{0};

    ProceduralTreeManager() = default;

    // Helper: Thêm hình trụ tròn mịn (Cylinder)
    static void addCylinder(std::vector<TreeVertex>& vertices, std::vector<unsigned int>& indices,
                            const glm::vec3& start, const glm::vec3& end,
                            float rStart, float rEnd, const glm::vec3& barkColor, int segments = 12) {
        glm::vec3 dir = end - start;
        float len = glm::length(dir);
        if (len < 0.001f) return;
        dir = glm::normalize(dir);

        glm::vec3 up = (std::abs(dir.y) < 0.95f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        glm::vec3 side = glm::normalize(glm::cross(dir, up));
        up = glm::normalize(glm::cross(side, dir));

        unsigned int baseIdx = static_cast<unsigned int>(vertices.size());

        for (int i = 0; i <= segments; ++i) {
            float angle = static_cast<float>(i) * (2.0f * 3.14159265f / segments);
            float c = std::cos(angle);
            float s = std::sin(angle);
            glm::vec3 n = glm::normalize(side * c + up * s);

            vertices.push_back({start + n * rStart, n, barkColor});
            vertices.push_back({end   + n * rEnd,   n, barkColor});
        }

        for (int i = 0; i < segments; ++i) {
            unsigned int b1 = baseIdx + i * 2;
            unsigned int t1 = b1 + 1;
            unsigned int b2 = b1 + 2;
            unsigned int t2 = b1 + 3;

            indices.push_back(b1); indices.push_back(b2); indices.push_back(t2);
            indices.push_back(b1); indices.push_back(t2); indices.push_back(t1);
        }
    }

    // Helper: Thêm khối tán lá hình cầu elip mượt mà (Ellipsoid Foliage Lobe)
    static void addFoliageLobe(std::vector<TreeVertex>& vertices, std::vector<unsigned int>& indices,
                               const glm::vec3& center, float rx, float ry, float rz,
                               const glm::vec3& color, int rings = 10, int sectors = 14) {
        unsigned int baseIdx = static_cast<unsigned int>(vertices.size());

        for (int i = 0; i <= rings; ++i) {
            float phi = static_cast<float>(i) * (3.14159265f / rings);
            float y = std::cos(phi);
            float sinPhi = std::sin(phi);

            // Shading gradient: đỉnh tán lá đón nắng sáng hơn, đáy hơi tối
            float sunShade = 0.85f + 0.25f * y;
            glm::vec3 lobeCol = glm::clamp(color * sunShade, 0.0f, 1.0f);

            for (int j = 0; j <= sectors; ++j) {
                float theta = static_cast<float>(j) * (2.0f * 3.14159265f / sectors);
                float x = sinPhi * std::cos(theta);
                float z = sinPhi * std::sin(theta);

                glm::vec3 norm = glm::normalize(glm::vec3(x / rx, y / ry, z / rz));
                glm::vec3 pos = center + glm::vec3(x * rx, y * ry, z * rz);

                vertices.push_back({pos, norm, lobeCol});
            }
        }

        for (int i = 0; i < rings; ++i) {
            for (int j = 0; j < sectors; ++j) {
                unsigned int cur  = baseIdx + i * (sectors + 1) + j;
                unsigned int next = cur + (sectors + 1);

                indices.push_back(cur);
                indices.push_back(next);
                indices.push_back(cur + 1);

                indices.push_back(cur + 1);
                indices.push_back(next);
                indices.push_back(next + 1);
            }
        }
    }

    // =========================================================================
    // 1. CÂY BÓNG MÁT TỰ NHIÊN HỮU CƠ (ORGANIC SHADE TREE - CHUẨN MẪU 3)
    // =========================================================================
    static void addOrganicShadeTree(std::vector<TreeVertex>& vertices, std::vector<unsigned int>& indices,
                                    const glm::vec3& basePos, float scale = 1.0f,
                                    const glm::vec3& leafColor = glm::vec3(0.28f, 0.68f, 0.18f)) {
        const glm::vec3 barkColor(0.48f, 0.35f, 0.25f);
        const glm::vec3 rootColor(0.42f, 0.30f, 0.20f);

        // a) 5 Rễ cây xòe rộng bò trên mặt cỏ (Flaring Buttress Roots - Mẫu 3)
        for (int r = 0; r < 5; ++r) {
            float ang = static_cast<float>(r) * (2.0f * 3.14159265f / 5.0f) + 0.2f * r;
            float ca = std::cos(ang), sa = std::sin(ang);
            glm::vec3 rootStart = basePos + glm::vec3(ca * 0.40f * scale, 0.35f * scale, sa * 0.40f * scale);
            glm::vec3 rootEnd   = basePos + glm::vec3(ca * 1.35f * scale, 0.02f, sa * 1.35f * scale);
            addCylinder(vertices, indices, rootStart, rootEnd, 0.22f * scale, 0.08f * scale, rootColor, 8);
        }

        // b) Thân chính uốn lượn tự nhiên
        glm::vec3 p0 = basePos;
        glm::vec3 p1 = basePos + glm::vec3( 0.05f * scale, 1.3f * scale, -0.05f * scale);
        glm::vec3 p2 = basePos + glm::vec3(-0.08f * scale, 2.6f * scale,  0.08f * scale);
        glm::vec3 p3 = basePos + glm::vec3( 0.00f * scale, 3.8f * scale,  0.00f * scale);

        addCylinder(vertices, indices, p0, p1, 0.52f * scale, 0.44f * scale, barkColor, 12);
        addCylinder(vertices, indices, p1, p2, 0.44f * scale, 0.36f * scale, barkColor, 12);
        addCylinder(vertices, indices, p2, p3, 0.36f * scale, 0.28f * scale, barkColor, 10);

        // c) 4 Cành chính phân bổ tự nhiên theo thuật toán Fractal (L-System Branching)
        struct Branch {
            glm::vec3 start;
            glm::vec3 end;
            float rStart;
            float rEnd;
        };

        std::vector<Branch> branches = {
            { p2, p2 + glm::vec3(-1.4f, 1.2f, -0.9f) * scale, 0.28f * scale, 0.16f * scale },
            { p2, p2 + glm::vec3( 1.5f, 1.1f, -0.7f) * scale, 0.27f * scale, 0.15f * scale },
            { p3, p3 + glm::vec3(-0.9f, 1.3f,  1.3f) * scale, 0.24f * scale, 0.14f * scale },
            { p3, p3 + glm::vec3( 1.1f, 1.4f,  1.0f) * scale, 0.24f * scale, 0.14f * scale },
            { p3, p3 + glm::vec3( 0.0f, 1.6f, -0.2f) * scale, 0.22f * scale, 0.13f * scale }
        };

        for (const auto& b : branches) {
            addCylinder(vertices, indices, b.start, b.end, b.rStart, b.rEnd, barkColor, 8);
        }

        // d) Các khối mây tán lá xum xuê kết chùm bồng bềnh (Puffy Foliage Clusters - Mẫu 3)
        // Tạo nên một tán cây đầy đặn, rực rỡ, mềm mại tự nhiên
        struct FoliageCluster {
            glm::vec3 offset;
            float rx, ry, rz;
        };

        std::vector<FoliageCluster> lobes = {
            // Khối tán trung tâm đỉnh
            { glm::vec3( 0.0f, 5.4f,  0.0f), 1.85f, 1.55f, 1.85f },
            { glm::vec3( 0.0f, 6.2f,  0.0f), 1.35f, 1.15f, 1.35f },

            // Tán cành Tây Bắc
            { glm::vec3(-1.5f, 4.2f, -1.1f), 1.45f, 1.25f, 1.45f },
            { glm::vec3(-1.8f, 4.8f, -0.8f), 1.20f, 1.10f, 1.20f },

            // Tán cành Đông Bắc
            { glm::vec3( 1.6f, 4.1f, -0.8f), 1.50f, 1.25f, 1.50f },
            { glm::vec3( 1.9f, 4.7f, -0.5f), 1.15f, 1.05f, 1.15f },

            // Tán cành Tây Nam
            { glm::vec3(-1.0f, 4.4f,  1.4f), 1.45f, 1.20f, 1.45f },
            { glm::vec3(-1.3f, 5.1f,  1.1f), 1.15f, 1.05f, 1.15f },

            // Tán cành Đông Nam
            { glm::vec3( 1.2f, 4.3f,  1.2f), 1.50f, 1.25f, 1.50f },
            { glm::vec3( 1.4f, 5.0f,  0.9f), 1.20f, 1.10f, 1.20f },

            // Khối lấp đầy vòm dưới
            { glm::vec3(-0.6f, 3.8f, -0.4f), 1.10f, 0.95f, 1.10f },
            { glm::vec3( 0.7f, 3.8f,  0.4f), 1.10f, 0.95f, 1.10f }
        };

        for (const auto& l : lobes) {
            glm::vec3 center = basePos + l.offset * scale;
            addFoliageLobe(vertices, indices, center, l.rx * scale, l.ry * scale, l.rz * scale, leafColor);
        }
    }

    // =========================================================================
    // 2. CÂY CỌ NGHỈ DƯỠNG NHIỆT ĐỚI (TROPICAL RESORT PALM - CHUẨN MẪU 1)
    // =========================================================================
    static void addResortPalm(std::vector<TreeVertex>& vertices, std::vector<unsigned int>& indices,
                              const glm::vec3& basePos, float trunkHeight = 8.0f,
                              float leanX = 0.8f, float leanZ = -0.4f) {
        const glm::vec3 trunkColor(0.44f, 0.35f, 0.28f);
        const glm::vec3 frondGreen(0.20f, 0.62f, 0.18f);

        constexpr int segments = 12;
        glm::vec3 prevPoint = basePos;
        float rStart = 0.36f;

        for (int i = 1; i <= segments; ++i) {
            float t = static_cast<float>(i) / segments;
            float y = basePos.y + t * trunkHeight;
            float curve = std::sin(t * 1.57f);
            glm::vec3 curPoint(basePos.x + curve * leanX, y, basePos.z + curve * leanZ);

            float rEnd = glm::mix(0.36f, 0.20f, t);
            addCylinder(vertices, indices, prevPoint, curPoint, rStart, rEnd, trunkColor, 8);

            prevPoint = curPoint;
            rStart = rEnd;
        }

        // Tán lá cọ xòe nan quạt uốn cong (12 tàu lá cọ)
        glm::vec3 crownTop = prevPoint;
        constexpr int numFronds = 12;
        for (int f = 0; f < numFronds; ++f) {
            float ang = static_cast<float>(f) * (2.0f * 3.14159265f / numFronds);
            float ca = std::cos(ang), sa = std::sin(ang);

            glm::vec3 pMid = crownTop + glm::vec3(ca * 2.2f, 0.5f, sa * 2.2f);
            glm::vec3 pTip = crownTop + glm::vec3(ca * 3.8f, -1.2f, sa * 3.8f);

            addCylinder(vertices, indices, crownTop, pMid, 0.08f, 0.05f, frondGreen, 4);
            addCylinder(vertices, indices, pMid, pTip, 0.05f, 0.02f, frondGreen, 4);

            // Phiến lá cọ dẹt màu xanh tươi
            unsigned int base = static_cast<unsigned int>(vertices.size());
            glm::vec3 side(-sa * 0.55f, 0.0f, ca * 0.55f);

            vertices.push_back({crownTop, {0, 1, 0}, frondGreen});
            vertices.push_back({pMid + side, {0, 1, 0}, frondGreen});
            vertices.push_back({pTip, {0, 1, 0}, frondGreen});
            vertices.push_back({pMid - side, {0, 1, 0}, frondGreen});

            indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
            indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
        }
    }

    // =========================================================================
    // 3. TÙNG THÁP ĐỊA TRUNG HẢI (MEDITERRANEAN PENCIL CYPRESS - CHUẨN MẪU 1)
    // =========================================================================
    static void addCypressTree(std::vector<TreeVertex>& vertices, std::vector<unsigned int>& indices,
                               const glm::vec3& basePos, float height = 8.5f, float maxRadius = 1.15f) {
        const glm::vec3 trunkColor(0.36f, 0.28f, 0.20f);
        const glm::vec3 cypressGreen(0.12f, 0.40f, 0.14f);

        addCylinder(vertices, indices, basePos, basePos + glm::vec3(0, 1.2f, 0), 0.25f, 0.20f, trunkColor, 6);

        constexpr int tiers = 8;
        float yStart = basePos.y + 1.0f;
        float hStep = (height - 1.0f) / tiers;

        for (int t = 0; t < tiers; ++t) {
            float frac = static_cast<float>(t) / tiers;
            float rTier = maxRadius * std::sin(frac * 3.14159f) + 0.35f;
            float yCenter = yStart + t * hStep + hStep * 0.5f;

            addFoliageLobe(vertices, indices, glm::vec3(basePos.x, yCenter, basePos.z),
                           rTier, hStep * 0.75f, rTier, cypressGreen, 6, 10);
        }
    }

    // =========================================================================
    // 4. BỤI CÂY CẦU XÉN TỈA TINH XẢO (TOPIARY SPHERES)
    // =========================================================================
    static void addTopiarySphere(std::vector<TreeVertex>& vertices, std::vector<unsigned int>& indices,
                                 const glm::vec3& center, float radius, const glm::vec3& color) {
        addCylinder(vertices, indices, glm::vec3(center.x, 0.0f, center.z), center, 0.06f, 0.06f, glm::vec3(0.3f, 0.2f, 0.1f), 6);
        addFoliageLobe(vertices, indices, center, radius, radius * 0.95f, radius, color, 8, 12);
    }

    void init() {
        std::vector<TreeVertex> vertices;
        std::vector<unsigned int> indices;

        // Bảng màu thực vật sang trọng
        const glm::vec3 lushGreen(0.28f, 0.68f, 0.18f);    // Xanh mướt tươi mát (Mẫu 3)
        const glm::vec3 deepGreen(0.18f, 0.52f, 0.15f);    // Xanh thẫm tự nhiên
        const glm::vec3 limeGreen(0.35f, 0.74f, 0.22f);    // Xanh non đón nắng
        const glm::vec3 mapleRed(0.92f, 0.18f, 0.15f);     // Phong lá đỏ nhấn cảnh
        const glm::vec3 topiaryGreen(0.16f, 0.48f, 0.16f); // Cây cắt tỉa

        // =====================================================================
        // 1. CÂY BÓNG MÁT TỰ NHIÊN HỮU CƠ (ORGANIC SHADE TREES - MẪU 3)
        // =====================================================================
        // Cây đại thụ trung tâm sân trong cạnh sảnh biệt thự (tương tự Mẫu 1 có cây ở sân thượng/sân trong)
        addOrganicShadeTree(vertices, indices, glm::vec3(-7.5f, 0.10f, 4.5f), 1.15f, lushGreen);

        // Cây bóng mát góc vườn tiền sảnh bên trái đài phun nước
        addOrganicShadeTree(vertices, indices, glm::vec3(-15.5f, 0.10f, 15.0f), 1.25f, deepGreen);

        // Cây bóng mát đón khách cạnh cổng vào
        addOrganicShadeTree(vertices, indices, glm::vec3(-12.5f, 0.10f, 27.5f), 1.10f, limeGreen);

        // Cây phong lá đỏ nghệ thuật bên cạnh hồ bơi
        addOrganicShadeTree(vertices, indices, glm::vec3(13.5f, 0.10f, 12.5f), 0.95f, mapleRed);

        // =====================================================================
        // 2. CÂY CỌ NGHỈ DƯỠNG BÊN HỒ BƠI VÔ CỰC (TROPICAL PALMS - MẪU 1)
        // =====================================================================
        addResortPalm(vertices, indices, glm::vec3(14.0f, 0.08f, -2.5f), 8.5f,  0.9f, -0.5f);
        addResortPalm(vertices, indices, glm::vec3(14.5f, 0.08f,  3.8f), 8.0f,  0.8f,  0.6f);
        addResortPalm(vertices, indices, glm::vec3( 8.2f, 0.08f, -6.5f), 7.6f, -0.6f, -0.6f);

        // =====================================================================
        // 3. HÀNG TÙNG THÁP ĐỊA TRUNG HẢI DỌC TƯỜNG RÀO (CYPRESS - MẪU 1)
        // =====================================================================
        // Dọc tường phía Tây (trái)
        for (int i = 0; i < 6; ++i) {
            float z = -20.0f + i * 8.5f;
            addCypressTree(vertices, indices, glm::vec3(-24.5f, 0.0f, z), 8.5f, 1.10f);
        }
        // Dọc tường phía Đông (phải)
        for (int i = 0; i < 6; ++i) {
            float z = -20.0f + i * 8.5f;
            addCypressTree(vertices, indices, glm::vec3(24.5f, 0.0f, z), 8.5f, 1.10f);
        }
        // Dọc tường phía Bắc (sau nhà)
        for (int i = 0; i < 6; ++i) {
            float x = -20.0f + i * 8.0f;
            addCypressTree(vertices, indices, glm::vec3(x, 0.0f, -24.5f), 9.0f, 1.20f);
        }

        // =====================================================================
        // 4. RỪNG CÂY TỰ NHIÊN NGOÀI KHUÔN VIÊN TRÊN ĐỒI PERLIN NOISE
        // =====================================================================
        std::vector<glm::vec3> outerForest = {
            {-34.0f, 1.5f, -20.0f}, {-36.0f, 2.0f,  -5.0f}, {-35.0f, 1.8f,  12.0f},
            { 34.0f, 1.5f, -20.0f}, { 36.0f, 2.0f,  -5.0f}, { 35.0f, 1.8f,  12.0f},
            {-28.0f, 1.2f, -34.0f}, { -8.0f, 2.2f, -35.0f}, { 12.0f, 2.0f, -34.0f}, { 28.0f, 1.2f, -34.0f}
        };
        for (size_t k = 0; k < outerForest.size(); ++k) {
            const auto& pos = outerForest[k];
            glm::vec3 col = (k % 2 == 0) ? deepGreen : lushGreen;
            addOrganicShadeTree(vertices, indices, pos, 1.35f, col);
        }

        // =====================================================================
        // 5. BỤI CÂY TRÒN XÉN TỈA TOPIARY DỌC HIÊN VÀ BỒN HOA
        // =====================================================================
        addTopiarySphere(vertices, indices, glm::vec3(-2.8f, 0.45f, 7.5f), 0.45f, topiaryGreen);
        addTopiarySphere(vertices, indices, glm::vec3( 2.8f, 0.45f, 7.5f), 0.45f, topiaryGreen);
        addTopiarySphere(vertices, indices, glm::vec3(-7.5f, 0.45f, 7.5f), 0.50f, topiaryGreen);
        addTopiarySphere(vertices, indices, glm::vec3( 7.5f, 0.45f, 7.5f), 0.50f, topiaryGreen);

        indexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TreeVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TreeVertex), (void*)offsetof(TreeVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TreeVertex), (void*)offsetof(TreeVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TreeVertex), (void*)offsetof(TreeVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~ProceduralTreeManager() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};

} // namespace modern
