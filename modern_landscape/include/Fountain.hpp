#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

namespace modern {

struct FountainVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @brief Class Fountain: Tháp phun nước nghệ thuật đa tầng & Bồn hoa tròn bao quanh
 * Tái hiện chính xác theo ảnh mẫu 2 (Image 2 - TDV Việt Nam):
 * 1. Bể tròn lớn đáy hồ (Outer Circular Basin) đường kính ~7.2m bằng đá vôi/travertine cao cấp.
 * 2. Vành hoa tròn bao quanh sát chân bể (Encircling Flower Ring) với hàng cây bụi cắt tỉa & hoa nở rực rỡ (hồng, đỏ, trắng, vàng).
 * 3. Trụ tháp trung tâm nâng đỡ 3 đĩa tràn uốn lượn loe miệng (3 Scalloped/Flared Bowls) thu nhỏ dần lên cao.
 * 4. Đỉnh tháp chóp búp sen/vòi phun trung tâm (Finial Spout).
 * 5. Màn tia nước vòm lấp lánh (Cascading Water Dome/Jets) phun từ các tầng rơi xuống hồ.
 */
class Fountain {
public:
    GLuint basinVAO{0}, basinVBO{0}, basinEBO{0};
    unsigned int basinIndexCount{0};

    GLuint waterVAO{0}, waterVBO{0}, waterEBO{0};
    unsigned int waterIndexCount{0};

    // Vị trí đặt đài phun nước tại trung tâm sân trước đối diện sảnh biệt thự
    glm::vec3 centerPos{-3.0f, 0.05f, 15.5f};
    float outerRadius{3.6f}; // Bán kính lòng hồ 3.6m

    Fountain() = default;

    void init() {
        initBasinAndSurroundingGarden();
        initWaterMesh();
    }

    void initBasinAndSurroundingGarden() {
        std::vector<FountainVertex> vertices;
        std::vector<unsigned int> indices;

        const glm::vec3 stoneCream(0.92f, 0.88f, 0.80f);   // Đá vôi Travertine vàng kem sang trọng (chuẩn Image 2)
        const glm::vec3 stoneShade(0.78f, 0.74f, 0.68f);   // Bóng đổ chi tiết đá
        const glm::vec3 darkGranite(0.24f, 0.25f, 0.28f);  // Lòng hồ đá xám sẫm
        const glm::vec3 patioTile(0.84f, 0.82f, 0.78f);    // Gạch lát sân quảng trường

        // Màu sắc thực vật và hoa bao quanh đài phun (Image 2)
        const glm::vec3 soilDark(0.22f, 0.16f, 0.12f);     // Đất mùn
        const glm::vec3 hedgeDark(0.14f, 0.42f, 0.14f);    // Bụi xanh đậm xén tròn
        const glm::vec3 hedgeLight(0.22f, 0.58f, 0.18f);   // Bụi xanh sáng
        const glm::vec3 flowerPink(0.96f, 0.28f, 0.62f);   // Hoa dạ yến thảo hồng sen
        const glm::vec3 flowerRed(0.95f, 0.10f, 0.12f);    // Hoa phong lữ đỏ rực
        const glm::vec3 flowerWhite(0.98f, 0.98f, 0.95f);  // Hoa tuyết cầu trắng tinh khôi
        const glm::vec3 flowerYellow(1.0f, 0.85f, 0.10f);  // Hoa cúc vàng kim

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

        // Helper: Thêm hình trụ tròn hoặc hình nón cụt (Frustum/Cylinder)
        auto addFrustum = [&](const glm::vec3& center, float rBot, float rTop, float height,
                              const glm::vec3& col, int segs = 32) {
            unsigned int baseIdx = static_cast<unsigned int>(vertices.size());
            float halfH = height * 0.5f;
            float yBot = center.y - halfH;
            float yTop = center.y + halfH;

            for (int i = 0; i <= segs; ++i) {
                float a = static_cast<float>(i) * (2.0f * 3.14159265f / segs);
                float ca = std::cos(a), sa = std::sin(a);
                glm::vec3 norm = glm::normalize(glm::vec3(ca, (rBot - rTop) / height, sa));
                vertices.push_back({{center.x + ca * rBot, yBot, center.z + sa * rBot}, norm, col});
                vertices.push_back({{center.x + ca * rTop, yTop, center.z + sa * rTop}, norm, col});
            }

            for (int i = 0; i < segs; ++i) {
                unsigned int b1 = baseIdx + i * 2;
                unsigned int t1 = b1 + 1;
                unsigned int b2 = b1 + 2;
                unsigned int t2 = b1 + 3;

                indices.push_back(b1); indices.push_back(b2); indices.push_back(t2);
                indices.push_back(b1); indices.push_back(t2); indices.push_back(t1);
            }
        };

        // Helper: Thêm đĩa vành tròn (Circular Ring / Washer)
        auto addRing = [&](const glm::vec3& center, float rInner, float rOuter, float y,
                           const glm::vec3& col, int segs = 32, bool up = true) {
            unsigned int baseIdx = static_cast<unsigned int>(vertices.size());
            glm::vec3 norm = up ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);

            for (int i = 0; i <= segs; ++i) {
                float a = static_cast<float>(i) * (2.0f * 3.14159265f / segs);
                float ca = std::cos(a), sa = std::sin(a);
                vertices.push_back({{center.x + ca * rInner, y, center.z + sa * rInner}, norm, col});
                vertices.push_back({{center.x + ca * rOuter, y, center.z + sa * rOuter}, norm, col});
            }

            for (int i = 0; i < segs; ++i) {
                unsigned int in1  = baseIdx + i * 2;
                unsigned int out1 = in1 + 1;
                unsigned int in2  = in1 + 2;
                unsigned int out2 = in1 + 3;

                if (up) {
                    indices.push_back(in1); indices.push_back(out1); indices.push_back(out2);
                    indices.push_back(in1); indices.push_back(out2); indices.push_back(in2);
                } else {
                    indices.push_back(in1); indices.push_back(out2); indices.push_back(out1);
                    indices.push_back(in1); indices.push_back(in2);  indices.push_back(out2);
                }
            }
        };

        // =====================================================================
        // 1. SÂN QUẢNG TRƯỜNG LÁT ĐÁ TRÒN (CIRCULAR PAVED PLAZA)
        // =====================================================================
        constexpr int segs = 36;
        addRing(centerPos, 0.0f, 6.2f, centerPos.y + 0.01f, patioTile, segs, true);

        // =====================================================================
        // 2. VÀNH VƯỜN HOA TRÒN BAO QUANH CHÂN BỂ (ENCIRCLING FLOWER PARTERRE - MẪU 2)
        // =====================================================================
        float rGardenIn  = 3.85f;
        float rGardenOut = 5.25f;

        // Gờ đá bao bọc luống hoa
        addFrustum(glm::vec3(centerPos.x, centerPos.y + 0.08f, centerPos.z), rGardenOut, rGardenOut + 0.08f, 0.16f, stoneShade, segs);
        addRing(centerPos, rGardenOut, rGardenOut + 0.12f, centerPos.y + 0.16f, stoneCream, segs, true);

        // Mặt đất mùn ẩm cho hoa
        addRing(centerPos, rGardenIn, rGardenOut, centerPos.y + 0.12f, soilDark, segs, true);

        // Hàng cây bụi xén tỉa và hoa xen kẽ đa sắc (Image 2)
        constexpr int numShrubs = 32;
        for (int i = 0; i < numShrubs; ++i) {
            float a = static_cast<float>(i) * (2.0f * 3.14159265f / numShrubs);
            float ca = std::cos(a), sa = std::sin(a);

            // Bụi cây xanh xén tròn đều đặn (Boxwood topiary bushes)
            float rHedge = 4.15f;
            float hx = centerPos.x + ca * rHedge;
            float hz = centerPos.z + sa * rHedge;
            addBox(hx, centerPos.y + 0.28f, hz, 0.40f, 0.32f, 0.40f, (i % 2 == 0) ? hedgeDark : hedgeLight);

            // Cụm hoa nở rực rỡ ở vành ngoài
            float rFlower = 4.70f;
            float fx = centerPos.x + ca * rFlower;
            float fz = centerPos.z + sa * rFlower;

            glm::vec3 fCol;
            int type = i % 4;
            if (type == 0) fCol = flowerPink;
            else if (type == 1) fCol = flowerRed;
            else if (type == 2) fCol = flowerWhite;
            else fCol = flowerYellow;

            // Thân/lá hoa
            addBox(fx, centerPos.y + 0.24f, fz, 0.36f, 0.24f, 0.36f, glm::vec3(0.20f, 0.50f, 0.18f));
            // Tán hoa rực rỡ xum xuê
            addBox(fx, centerPos.y + 0.38f, fz, 0.42f, 0.18f, 0.42f, fCol);
        }

        // =====================================================================
        // 3. THÀNH BỂ HỒ CHÍNH TẦNG 1 (LOWER CIRCULAR STONE BASIN)
        // =====================================================================
        float basinWallThick = 0.25f;
        float basinH = 0.52f;
        float rInner = outerRadius - basinWallThick;
        float yBasinMid = centerPos.y + basinH * 0.5f;

        // Thành ngoài bể
        addFrustum(glm::vec3(centerPos.x, yBasinMid, centerPos.z), outerRadius, outerRadius, basinH, stoneCream, segs);
        // Thành trong bể
        addFrustum(glm::vec3(centerPos.x, yBasinMid, centerPos.z), rInner, rInner, basinH, stoneShade, segs);
        // Mặt trên thành bể bo tròn êm ái
        addRing(centerPos, rInner, outerRadius, centerPos.y + basinH, stoneCream, segs, true);
        // Đáy hồ đá sẫm
        addRing(centerPos, 0.0f, rInner, centerPos.y + 0.06f, darkGranite, segs, true);

        // =====================================================================
        // 4. TRỤ THÁP TRUNG TÂM (CENTRAL SCULPTED PEDESTAL)
        // =====================================================================
        // Chân đế trụ
        addFrustum(glm::vec3(centerPos.x, centerPos.y + 0.30f, centerPos.z), 0.95f, 0.75f, 0.48f, stoneCream, 24);
        addRing(centerPos, 0.0f, 0.75f, centerPos.y + 0.54f, stoneCream, 24, true);

        // Trụ tháp tầng 1 (nâng đỡ đĩa 1)
        addFrustum(glm::vec3(centerPos.x, centerPos.y + 0.85f, centerPos.z), 0.55f, 0.42f, 0.62f, stoneCream, 24);

        // =====================================================================
        // 5. ĐĨA TRÀN TẦNG 1 (LOWER SCALLOPED BOWL - R = 1.95m, Y = 1.25m)
        // =====================================================================
        float yBowl1 = centerPos.y + 1.22f;
        addFrustum(glm::vec3(centerPos.x, yBowl1 - 0.16f, centerPos.z), 0.50f, 1.95f, 0.32f, stoneCream, 32);
        // Lòng đĩa hứng nước
        addFrustum(glm::vec3(centerPos.x, yBowl1 + 0.04f, centerPos.z), 1.95f, 1.82f, 0.16f, stoneShade, 32);
        addRing(centerPos, 0.40f, 1.82f, yBowl1 - 0.04f, stoneShade, 32, true);

        // Trụ tháp tầng 2 (nâng đỡ đĩa 2)
        addFrustum(glm::vec3(centerPos.x, yBowl1 + 0.45f, centerPos.z), 0.40f, 0.30f, 0.74f, stoneCream, 20);

        // =====================================================================
        // 6. ĐĨA TRÀN TẦNG 2 (MIDDLE SCALLOPED BOWL - R = 1.25m, Y = 2.05m)
        // =====================================================================
        float yBowl2 = yBowl1 + 0.82f;
        addFrustum(glm::vec3(centerPos.x, yBowl2 - 0.14f, centerPos.z), 0.35f, 1.25f, 0.28f, stoneCream, 28);
        addFrustum(glm::vec3(centerPos.x, yBowl2 + 0.03f, centerPos.z), 1.25f, 1.15f, 0.14f, stoneShade, 28);
        addRing(centerPos, 0.28f, 1.15f, yBowl2 - 0.04f, stoneShade, 28, true);

        // Trụ tháp tầng 3 (nâng đỡ đĩa 3)
        addFrustum(glm::vec3(centerPos.x, yBowl2 + 0.38f, centerPos.z), 0.26f, 0.18f, 0.62f, stoneCream, 16);

        // =====================================================================
        // 7. ĐĨA TRÀN TẦNG 3 (TOP SCALLOPED BOWL - R = 0.70m, Y = 2.75m)
        // =====================================================================
        float yBowl3 = yBowl2 + 0.69f;
        addFrustum(glm::vec3(centerPos.x, yBowl3 - 0.10f, centerPos.z), 0.22f, 0.70f, 0.20f, stoneCream, 24);
        addFrustum(glm::vec3(centerPos.x, yBowl3 + 0.03f, centerPos.z), 0.70f, 0.62f, 0.10f, stoneCream, 24);
        addRing(centerPos, 0.16f, 0.62f, yBowl3 - 0.02f, stoneShade, 24, true);

        // Chóp búp sen đỉnh tháp (Finial Spout)
        addFrustum(glm::vec3(centerPos.x, yBowl3 + 0.25f, centerPos.z), 0.18f, 0.04f, 0.40f, stoneCream, 16);

        basinIndexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &basinVAO);
        glGenBuffers(1, &basinVBO);
        glGenBuffers(1, &basinEBO);

        glBindVertexArray(basinVAO);
        glBindBuffer(GL_ARRAY_BUFFER, basinVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(FountainVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, basinEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FountainVertex), (void*)offsetof(FountainVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FountainVertex), (void*)offsetof(FountainVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FountainVertex), (void*)offsetof(FountainVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void initWaterMesh() {
        std::vector<FountainVertex> vertices;
        std::vector<unsigned int> indices;

        const glm::vec3 waterSurfaceCol(0.15f, 0.85f, 0.98f); // Nước xanh ngọc trong vắt
        const glm::vec3 waterSprayCol(0.85f, 0.96f, 1.0f);    // Bọt nước trắng xóa lấp lánh (Mẫu 2)

        // Helper: Thêm đĩa tròn mặt nước
        auto addWaterCircle = [&](const glm::vec3& center, float r, float y, int segs = 32) {
            unsigned int centerIdx = static_cast<unsigned int>(vertices.size());
            vertices.push_back({{center.x, y, center.z}, {0, 1, 0}, waterSurfaceCol});
            for (int i = 0; i <= segs; ++i) {
                float a = static_cast<float>(i) * (2.0f * 3.14159265f / segs);
                vertices.push_back({{center.x + std::cos(a) * r, y, center.z + std::sin(a) * r}, {0, 1, 0}, waterSurfaceCol});
            }
            for (int i = 0; i < segs; ++i) {
                indices.push_back(centerIdx);
                indices.push_back(centerIdx + 1 + i);
                indices.push_back(centerIdx + 2 + i);
            }
        };

        // 1. Mặt nước hồ chính tầng 1
        addWaterCircle(centerPos, outerRadius - 0.28f, centerPos.y + 0.40f, 36);

        // 2. Mặt nước đĩa tràn tầng 1, 2, 3
        float yBowl1 = centerPos.y + 1.22f;
        float yBowl2 = yBowl1 + 0.82f;
        float yBowl3 = yBowl2 + 0.69f;
        addWaterCircle(centerPos, 1.80f, yBowl1 + 0.02f, 28);
        addWaterCircle(centerPos, 1.12f, yBowl2 + 0.02f, 24);
        addWaterCircle(centerPos, 0.60f, yBowl3 + 0.02f, 16);

        // =====================================================================
        // 3. VÒM TIA NƯỚC HÌNH NÓN CHUÔNG RƠI TỰ DO (WATER SPRAY DOME - CHUẨN MẪU 2)
        // Mẫu 2 có hàng chục tia nước phun từ đĩa trên tỏa đều xuống đĩa dưới và hồ chính
        // =====================================================================
        constexpr int numJets = 36;
        for (int j = 0; j < numJets; ++j) {
            float a = static_cast<float>(j) * (2.0f * 3.14159265f / numJets);
            float ca = std::cos(a), sa = std::sin(a);

            // Màn tia nước vòm từ Đĩa tầng 2 (R=1.2m) đổ xuống Hồ tầng 1 (R=3.2m)
            constexpr int steps = 8;
            for (int s = 0; s < steps; ++s) {
                float t1 = static_cast<float>(s) / static_cast<float>(steps);
                float t2 = static_cast<float>(s + 1) / static_cast<float>(steps);

                // Quỹ đạo parabol đường đạn (ballistic trajectory)
                float r1 = glm::mix(1.20f, 3.25f, t1);
                float y1 = glm::mix(yBowl2, centerPos.y + 0.42f, t1 * t1);

                float r2 = glm::mix(1.20f, 3.25f, t2);
                float y2 = glm::mix(yBowl2, centerPos.y + 0.42f, t2 * t2);

                glm::vec3 p1 = centerPos + glm::vec3(ca * r1, y1, sa * r1);
                glm::vec3 p2 = centerPos + glm::vec3(ca * r2, y2, sa * r2);

                unsigned int base = static_cast<unsigned int>(vertices.size());
                float w = 0.045f;
                glm::vec3 side(-sa * w, 0.0f, ca * w);

                vertices.push_back({p1 - side, {0, 1, 0}, waterSprayCol});
                vertices.push_back({p1 + side, {0, 1, 0}, waterSprayCol});
                vertices.push_back({p2 + side, {0, 1, 0}, waterSprayCol});
                vertices.push_back({p2 - side, {0, 1, 0}, waterSprayCol});

                indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
                indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
            }

            // Màn tia nước nhỏ từ Đĩa tầng 3 (R=0.68m) đổ vào Đĩa tầng 2 (R=1.15m)
            for (int s = 0; s < 4; ++s) {
                float t1 = static_cast<float>(s) / 4.0f;
                float t2 = static_cast<float>(s + 1) / 4.0f;

                float r1 = glm::mix(0.68f, 1.15f, t1);
                float y1 = glm::mix(yBowl3, yBowl2 + 0.05f, t1 * t1);
                float r2 = glm::mix(0.68f, 1.15f, t2);
                float y2 = glm::mix(yBowl3, yBowl2 + 0.05f, t2 * t2);

                glm::vec3 p1 = centerPos + glm::vec3(ca * r1, y1, sa * r1);
                glm::vec3 p2 = centerPos + glm::vec3(ca * r2, y2, sa * r2);

                unsigned int base = static_cast<unsigned int>(vertices.size());
                float w = 0.035f;
                glm::vec3 side(-sa * w, 0.0f, ca * w);

                vertices.push_back({p1 - side, {0, 1, 0}, waterSprayCol});
                vertices.push_back({p1 + side, {0, 1, 0}, waterSprayCol});
                vertices.push_back({p2 + side, {0, 1, 0}, waterSprayCol});
                vertices.push_back({p2 - side, {0, 1, 0}, waterSprayCol});

                indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
                indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
            }
        }

        // Cột nước phun vút lên từ chóp đỉnh
        for (int s = 0; s < 6; ++s) {
            float ang = static_cast<float>(s) * (2.0f * 3.14159265f / 6.0f);
            float ca = std::cos(ang), sa = std::sin(ang);
            glm::vec3 p1 = centerPos + glm::vec3(0, yBowl3 + 0.35f, 0);
            glm::vec3 p2 = centerPos + glm::vec3(ca * 0.25f, yBowl3 + 0.95f, sa * 0.25f);

            unsigned int base = static_cast<unsigned int>(vertices.size());
            vertices.push_back({p1 + glm::vec3(-0.04f, 0, 0), {0, 1, 0}, waterSprayCol});
            vertices.push_back({p1 + glm::vec3( 0.04f, 0, 0), {0, 1, 0}, waterSprayCol});
            vertices.push_back({p2 + glm::vec3( 0.04f, 0, 0), {0, 1, 0}, waterSprayCol});
            vertices.push_back({p2 + glm::vec3(-0.04f, 0, 0), {0, 1, 0}, waterSprayCol});

            indices.push_back(base + 0); indices.push_back(base + 1); indices.push_back(base + 2);
            indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
        }

        waterIndexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &waterVAO);
        glGenBuffers(1, &waterVBO);
        glGenBuffers(1, &waterEBO);

        glBindVertexArray(waterVAO);
        glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(FountainVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, waterEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(FountainVertex), (void*)offsetof(FountainVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(FountainVertex), (void*)offsetof(FountainVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(FountainVertex), (void*)offsetof(FountainVertex, color));
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

    ~Fountain() {
        if (basinVAO) glDeleteVertexArrays(1, &basinVAO);
        if (basinVBO) glDeleteBuffers(1, &basinVBO);
        if (basinEBO) glDeleteBuffers(1, &basinEBO);
        if (waterVAO) glDeleteVertexArrays(1, &waterVAO);
        if (waterVBO) glDeleteBuffers(1, &waterVBO);
        if (waterEBO) glDeleteBuffers(1, &waterEBO);
    }
};

} // namespace modern
