#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

namespace modern {

struct WallVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @brief Class ModernWalls: Tường rào phủ cây xanh, Đường phố mặt tiền & Sân đỗ xe
 * Tái hiện quy hoạch kiến trúc tổng thể từ ảnh mẫu 1 (Image 1 - XHOME Villa Estate):
 * 1. TRỤC ĐƯỜNG MẶT TIỀN & VỈA HÈ HIỆN ĐẠI (Front Street & Sidewalk):
 *    - Lòng đường nhựa Asphalt 2 làn xe với vạch kẻ đường đứt đoạn màu trắng phản quang.
 *    - Vỉa hè đi bộ lát gạch sạch sẽ có gờ bó vỉa bê tông.
 * 2. TƯỜNG RÀO PHỦ MẢNG CÂY XANH / VERTICAL GARDEN (Ivy Living Wall):
 *    - Mặt ngoài tường rào phủ thảm dây leo xanh ngắt (chuẩn mẫu 1).
 *    - Nẹp chỉ kim loại đen xám than và nan gỗ Teak sang trọng.
 * 3. CỔNG BIỆT THỰ & ĐÈN HẮT LED:
 *    - Trụ cổng đá granite, đèn hắt tường LED vàng ấm và cánh cổng trượt thép vân gỗ.
 * 4. SÂN ĐỖ XE / CARPORT PAVEMENT:
 *    - Sân lát đá phiến tự nhiên bên phải đón xe từ cổng vào.
 */
class ModernWalls {
public:
    GLuint VAO{0}, VBO{0}, EBO{0};
    unsigned int indexCount{0};

    ModernWalls() = default;

    void init() {
        std::vector<WallVertex> vertices;
        std::vector<unsigned int> indices;

        const glm::vec3 asphaltRoad(0.28f, 0.28f, 0.30f);   // Đường nhựa Asphalt
        const glm::vec3 roadStripe(0.98f, 0.98f, 0.98f);    // Vạch sơn đường trắng
        const glm::vec3 sidewalkTile(0.82f, 0.80f, 0.76f);  // Gạch lát vỉa hè
        const glm::vec3 curbColor(0.70f, 0.70f, 0.72f);     // Bó vỉa bê tông
        const glm::vec3 drivewayTile(0.78f, 0.76f, 0.73f);  // Đá lát sân đỗ xe (Image 1)

        const glm::vec3 wallConcrete(0.92f, 0.92f, 0.90f); // Tường trắng hiện đại
        const glm::vec3 wallCap(0.25f, 0.25f, 0.28f);      // Nắp chụp xám than
        const glm::vec3 woodSlats(0.72f, 0.44f, 0.22f);    // Nan gỗ teak chống thấm
        const glm::vec3 blackSteel(0.15f, 0.15f, 0.18f);   // Cổng thép sơn tĩnh điện đen
        const glm::vec3 gateLight(0.98f, 0.85f, 0.35f);    // Đèn LED cổng ấm áp
        const glm::vec3 ivyGreen(0.16f, 0.46f, 0.16f);     // Thảm dây leo phủ tường (Mẫu 1)

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

        // =====================================================================
        // 1. TRỤC ĐƯỜNG MẶT TIỀN & VỈA HÈ ĐI BỘ (FRONT STREET - CHUẨN MẪU 1)
        // =====================================================================
        // Lòng đường nhựa Asphalt (Z từ 35.8m đến 43.0m, rộng 7.2m)
        addBox(0.0f, 0.01f, 39.4f, 65.0f, 0.02f, 7.2f, asphaltRoad);

        // Vạch kẻ tim đường đứt đoạn (White Dashed Center Lines)
        for (int k = -5; k <= 5; ++k) {
            float sx = k * 5.5f;
            addBox(sx, 0.025f, 39.4f, 3.2f, 0.01f, 0.22f, roadStripe);
        }

        // Vỉa hè đi bộ lát đá (Z từ 32.5m đến 35.6m, rộng 3.1m)
        addBox(0.0f, 0.06f, 34.05f, 60.0f, 0.12f, 3.1f, sidewalkTile);

        // Gờ bó vỉa bê tông phân cách lòng đường và vỉa hè
        addBox(0.0f, 0.10f, 35.65f, 60.0f, 0.20f, 0.25f, curbColor);

        // =====================================================================
        // 2. SÂN ĐỖ XE / CARPORT PAVEMENT BÊN TRONG KHUÔN VIÊN (MẪU 1)
        // Khu vực bên phải sân trước (X từ 13.0m đến 24.5m, Z từ 18.0m đến 31.0m)
        // =====================================================================
        addBox(18.75f, 0.03f, 24.5f, 11.5f, 0.04f, 13.0f, drivewayTile);

        // Đường chỉ gạch trang trí sân đỗ xe
        for (int r = 0; r < 6; ++r) {
            float zSlot = 19.5f + r * 2.2f;
            addBox(18.75f, 0.045f, zSlot, 11.2f, 0.02f, 0.10f, wallCap);
        }

        // =====================================================================
        // 3. TƯỜNG RÀO KIẾN TRÚC PHỦ THẢM CÂY XANH (LIVING IVY WALLS - MẪU 1)
        // =====================================================================
        float wallH = 2.5f;
        float wallThick = 0.35f;

        auto addWallSectionWithIvy = [&](float x, float y, float z, float w, float h, float d, bool horizontal, bool frontFacing = false) {
            // Thân tường bê tông
            addBox(x, y + h * 0.5f, z, w, h, d, wallConcrete);
            // Nắp chụp kim loại trên đỉnh
            float capW = horizontal ? w : w + 0.12f;
            float capD = horizontal ? d + 0.12f : d;
            addBox(x, y + h + 0.05f, z, capW, 0.10f, capD, wallCap);

            // Mảng thảm cây xanh / dây leo bao phủ mặt ngoài tường (như trong Mẫu 1)
            if (frontFacing) {
                addBox(x, y + h * 0.5f, z + d * 0.55f, w, h * 0.95f, 0.12f, ivyGreen);
            }

            // Nan gỗ Teak trang trí mặt trong tường
            if (horizontal && w > 4.0f) {
                int slatCount = static_cast<int>(w * 1.4f);
                float slatStep = w / (slatCount + 1);
                for (int s = 1; s <= slatCount; ++s) {
                    float sx = x - w * 0.5f + s * slatStep;
                    addBox(sx, y + h * 0.5f, z - d * 0.52f, 0.06f, h * 0.75f, 0.04f, woodSlats);
                }
            }
        };

        // Tường phía sau Bắc (Z = -26.0f)
        addWallSectionWithIvy(0.0f, 0.0f, -26.0f, 52.0f, wallH, wallThick, true, false);

        // Tường phía Tây bên trái (X = -26.0f)
        addWallSectionWithIvy(-26.0f, 0.0f, 3.0f, wallThick, wallH, 58.0f, false, false);

        // Tường phía Đông bên phải (X = 26.0f)
        addWallSectionWithIvy(26.0f, 0.0f, 3.0f, wallThick, wallH, 58.0f, false, false);

        // Tường mặt tiền Nam (Z = 32.0f, có phủ thảm xanh dây leo Mẫu 1 và cổng lớn tại X = -18.0f)
        // Đoạn tường trước bên trái (từ -26.0m đến -20.5m)
        addWallSectionWithIvy(-23.25f, 0.0f, 32.0f, 5.5f, wallH, wallThick, true, true);

        // Đoạn tường trước bên phải (từ -15.5m đến 26.0m)
        addWallSectionWithIvy(5.25f, 0.0f, 32.0f, 41.5f, wallH, wallThick, true, true);

        // =====================================================================
        // 4. CỔNG BIỆT THỰ LUXURY TẠI X = -18.0, Z = 32.0 (CHUẨN MẪU 1)
        // =====================================================================
        // Trụ cổng trái (-20.5, 32.0)
        addBox(-20.5f, 1.6f, 32.0f, 1.15f, 3.20f, 1.15f, wallCap);
        addBox(-20.5f, 3.28f, 32.0f, 1.35f, 0.16f, 1.35f, wallConcrete);
        // Đèn hắt tường LED trụ trái
        addBox(-20.5f, 2.2f, 32.65f, 0.22f, 0.35f, 0.12f, gateLight);

        // Trụ cổng phải (-15.5, 32.0)
        addBox(-15.5f, 1.6f, 32.0f, 1.15f, 3.20f, 1.15f, wallCap);
        addBox(-15.5f, 3.28f, 32.0f, 1.35f, 0.16f, 1.35f, wallConcrete);
        // Đèn hắt tường LED trụ phải
        addBox(-15.5f, 2.2f, 32.65f, 0.22f, 0.35f, 0.12f, gateLight);

        // Cánh cổng trượt tự động bằng khung thép đen & nan gỗ teak sang trọng
        addBox(-19.3f, 1.25f, 31.8f, 2.2f, 2.30f, 0.10f, blackSteel);
        addBox(-16.7f, 1.25f, 31.2f, 2.2f, 2.30f, 0.10f, blackSteel);

        // Nan gỗ ốp ngang trên cánh cổng
        for (int i = 0; i < 7; ++i) {
            float ySlot = 0.4f + i * 0.30f;
            addBox(-19.3f, ySlot, 31.86f, 2.0f, 0.14f, 0.05f, woodSlats);
            addBox(-16.7f, ySlot, 31.26f, 2.0f, 0.14f, 0.05f, woodSlats);
        }

        indexCount = static_cast<unsigned int>(indices.size());

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(WallVertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(WallVertex), (void*)offsetof(WallVertex, position));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(WallVertex), (void*)offsetof(WallVertex, normal));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(WallVertex), (void*)offsetof(WallVertex, color));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    ~ModernWalls() {
        if (VAO) glDeleteVertexArrays(1, &VAO);
        if (VBO) glDeleteBuffers(1, &VBO);
        if (EBO) glDeleteBuffers(1, &EBO);
    }
};

} // namespace modern
