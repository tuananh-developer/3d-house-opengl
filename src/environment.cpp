#include "house/scene.hpp"
#include "house/render_utils.hpp"
#include <GL/freeglut.h>
#include <cmath>

namespace house {
namespace {

constexpr float PI = 3.14159265358979323846F;

// Helper: Vẽ hình kim tự tháp 4 mặt làm chóp trang trí
void drawPyramidCap(float x, float y, float z, float width, float height, float depth,
                    float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    setMaterial(r, g, b, 28.0F);

    const float hw = width * 0.5F;
    const float hd = depth * 0.5F;

    glBegin(GL_TRIANGLES);
        glNormal3f(0.0F, hw, -height);
        glVertex3f(-hw, 0.0F, -hd);
        glVertex3f(0.0F, height, 0.0F);
        glVertex3f(hw, 0.0F, -hd);

        glNormal3f(0.0F, hw, height);
        glVertex3f(hw, 0.0F, hd);
        glVertex3f(0.0F, height, 0.0F);
        glVertex3f(-hw, 0.0F, hd);

        glNormal3f(-height, hd, 0.0F);
        glVertex3f(-hw, 0.0F, hd);
        glVertex3f(0.0F, height, 0.0F);
        glVertex3f(-hw, 0.0F, -hd);

        glNormal3f(height, hd, 0.0F);
        glVertex3f(hw, 0.0F, -hd);
        glVertex3f(0.0F, height, 0.0F);
        glVertex3f(hw, 0.0F, hd);
    glEnd();
    glPopMatrix();
}

// Helper: Vẽ một đóa hoa tinh xảo có cuống, lá xanh, cánh hoa nổi và nhụy hoa
void drawFlower(float x, float y, float z, float r, float g, float b, float stemH = 0.22F) {
    // Cuống hoa xanh mảnh
    drawBox(x, y + stemH * 0.5F, z, 0.02F, stemH, 0.02F, 0.18F, 0.50F, 0.16F);

    // Lá xanh nhỏ ở nách cuống
    glPushMatrix();
    glTranslatef(x + 0.025F, y + stemH * 0.4F, z);
    glRotatef(30.0F, 0.0F, 0.0F, 1.0F);
    drawBox(0.0F, 0.0F, 0.0F, 0.05F, 0.012F, 0.035F, 0.16F, 0.44F, 0.14F);
    glPopMatrix();

    // Cánh hoa hình cầu nổi bật
    glPushMatrix();
    glTranslatef(x, y + stemH + 0.02F, z);
    setMaterial(r, g, b, 26.0F);
    glutSolidSphere(0.068, 10, 8);

    // Nhụy hoa màu vàng cam ấm áp
    glTranslatef(0.0F, 0.025F, 0.0F);
    setMaterial(0.98F, 0.85F, 0.18F, 36.0F);
    glutSolidSphere(0.032, 8, 6);
    glPopMatrix();
}

// Helper: Vẽ hoa dại điểm xuyết trên thảm cỏ
void drawWildflower(float x, float y, float z, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y + 0.035F, z);
    setMaterial(r, g, b, 32.0F);
    glutSolidSphere(0.045, 8, 6);
    glPopMatrix();
}

// Helper: Vẽ bụi cây cảnh cắt tỉa (topiary shrub)
void drawShrub(float x, float y, float z, float radius, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y + radius * 0.75F, z);
    glScalef(1.0F, 0.85F, 1.0F);
    setMaterial(r, g, b, 12.0F);
    glutSolidSphere(radius, 14, 10);
    glPopMatrix();
}

// Helper: Vẽ bụi hoa cẩm tú cầu nở rộ
void drawHydrangeaBush(float x, float z, float radius, float flowerR, float flowerG, float flowerB) {
    drawShrub(x, 0.0F, z, radius, 0.14F, 0.40F, 0.15F);
    const float clusters[][3] = {
        { 0.0F, radius * 1.32F, 0.0F},
        {-radius * 0.55F, radius * 1.08F,  radius * 0.42F},
        { radius * 0.55F, radius * 1.10F,  radius * 0.42F},
        {-radius * 0.50F, radius * 1.04F, -radius * 0.42F},
        { radius * 0.50F, radius * 1.06F, -radius * 0.42F}
    };
    for (const auto& c : clusters) {
        glPushMatrix();
        glTranslatef(x + c[0], c[1], z + c[2]);
        setMaterial(flowerR, flowerG, flowerB, 24.0F);
        glutSolidSphere(0.12, 10, 8);
        glPopMatrix();
    }
}

// Helper: Vẽ ghế đá/gỗ sân vườn kiểu công viên (Park Bench)
void drawGardenBench(float x, float z, float angle) {
    glPushMatrix();
    glTranslatef(x, 0.0F, z);
    glRotatef(angle, 0.0F, 1.0F, 0.0F);

    // Chân sắt đúc màu đen xám
    const float ironR = 0.20F, ironG = 0.20F, ironB = 0.22F;
    drawBox(-0.65F, 0.24F, -0.16F, 0.06F, 0.48F, 0.06F, ironR, ironG, ironB);
    drawBox(-0.65F, 0.24F,  0.16F, 0.06F, 0.48F, 0.06F, ironR, ironG, ironB);
    drawBox( 0.65F, 0.24F, -0.16F, 0.06F, 0.48F, 0.06F, ironR, ironG, ironB);
    drawBox( 0.65F, 0.24F,  0.16F, 0.06F, 0.48F, 0.06F, ironR, ironG, ironB);

    // Nan gỗ màu nâu vàng ấm
    const float woodR = 0.65F, woodG = 0.42F, woodB = 0.22F;
    // Mặt ngồi
    drawBox(0.0F, 0.48F, -0.10F, 1.50F, 0.04F, 0.12F, woodR, woodG, woodB);
    drawBox(0.0F, 0.48F,  0.05F, 1.50F, 0.04F, 0.12F, woodR, woodG, woodB);
    drawBox(0.0F, 0.48F,  0.20F, 1.50F, 0.04F, 0.12F, woodR, woodG, woodB);
    // Tựa lưng
    drawBox(0.0F, 0.72F, 0.24F, 1.50F, 0.10F, 0.035F, woodR, woodG, woodB);
    drawBox(0.0F, 0.88F, 0.24F, 1.50F, 0.10F, 0.035F, woodR, woodG, woodB);

    glPopMatrix();
}

// Helper: Vẽ đèn lồng sân vườn trang nhã
void drawGardenLight(float x, float z) {
    drawBox(x, 0.25F, z, 0.10F, 0.50F, 0.10F, 0.18F, 0.18F, 0.22F);
    drawBox(x, 0.54F, z, 0.14F, 0.12F, 0.14F, 0.98F, 0.82F, 0.32F);
    drawBox(x, 0.62F, z, 0.18F, 0.04F, 0.18F, 0.22F, 0.22F, 0.25F);
    drawPyramidCap(x, 0.64F, z, 0.18F, 0.06F, 0.18F, 0.22F, 0.22F, 0.25F);
}

// =============================================================================
// ĐÀI PHUN NƯỚC NGHỆ THUẬT TRÁNG LỆ (GRAND ARCHITECTURAL FOUNTAIN)
// =============================================================================
void drawGrandFountain(float centerX, float centerZ) {
    glPushMatrix();
    glTranslatef(centerX, 0.0F, centerZ);

    // 1. Đế móng quảng trường tròn lát đá cẩm thạch bao quanh đài phun nước
    constexpr int plazaSegments = 24;
    constexpr float plazaRadius = 3.6F;
    setMaterial(0.76F, 0.74F, 0.70F, 16.0F);
    glBegin(GL_POLYGON);
        glNormal3f(0.0F, 1.0F, 0.0F);
        for (int i = 0; i < plazaSegments; ++i) {
            const float a = static_cast<float>(i) * (2.0F * PI / static_cast<float>(plazaSegments));
            glVertex3f(std::cos(a) * plazaRadius, 0.02F, std::sin(a) * plazaRadius);
        }
    glEnd();

    // Viền đá viền ngoài quảng trường
    setMaterial(0.48F, 0.46F, 0.42F, 24.0F);
    glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= plazaSegments; ++i) {
            const float a = static_cast<float>(i % plazaSegments) * (2.0F * PI / static_cast<float>(plazaSegments));
            const float c = std::cos(a), s = std::sin(a);
            glNormal3f(c, 0.4F, s);
            glVertex3f(c * plazaRadius, 0.02F, s * plazaRadius);
            glVertex3f(c * (plazaRadius + 0.18F), 0.035F, s * (plazaRadius + 0.18F));
        }
    glEnd();

    // 2. Thành bể hồ nước lớn tầng 1 (Octagonal / Cylindrical Lower Pool Wall)
    constexpr int poolSegments = 16;
    constexpr float poolRadius = 2.10F;
    constexpr float wallHeight = 0.48F;
    const float wallR = 0.82F, wallG = 0.80F, wallB = 0.75F; // Đá hoa cương cẩm thạch sáng

    // Thành ngoài hồ nước
    for (int i = 0; i < poolSegments; ++i) {
        const float a1 = static_cast<float>(i) * (2.0F * PI / static_cast<float>(poolSegments));
        const float a2 = static_cast<float>(i + 1) * (2.0F * PI / static_cast<float>(poolSegments));
        const float x1 = std::cos(a1) * poolRadius, z1 = std::sin(a1) * poolRadius;
        const float x2 = std::cos(a2) * poolRadius, z2 = std::sin(a2) * poolRadius;
        const float mx = (x1 + x2) * 0.5F, mz = (z1 + z2) * 0.5F;
        const float len = std::sqrt((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));
        const float ang = std::atan2(x2 - x1, z2 - z1) * (180.0F / PI);

        glPushMatrix();
        glTranslatef(mx, wallHeight * 0.5F + 0.02F, mz);
        glRotatef(ang - 90.0F, 0.0F, 1.0F, 0.0F);
        // Khối thành hồ và gờ phào chỉ mặt trên
        drawBox(0.0F, 0.0F, 0.0F, len * 1.02F, wallHeight, 0.16F, wallR, wallG, wallB);
        drawBox(0.0F, wallHeight * 0.5F + 0.02F, 0.0F, len * 1.05F, 0.04F, 0.22F, 0.90F, 0.88F, 0.82F);
        glPopMatrix();
    }

    // 3. Mặt nước xanh ngọc trong vắt của hồ tầng 1 (Deep Aquamarine Water Surface)
    setMaterial(0.20F, 0.68F, 0.85F, 96.0F); // Độ bóng cao phản chiếu ánh sáng nước
    glBegin(GL_POLYGON);
        glNormal3f(0.0F, 1.0F, 0.0F);
        for (int i = 0; i < poolSegments; ++i) {
            const float a = static_cast<float>(i) * (2.0F * PI / static_cast<float>(poolSegments));
            glVertex3f(std::cos(a) * (poolRadius - 0.08F), wallHeight * 0.72F, std::sin(a) * (poolRadius - 0.08F));
        }
    glEnd();

    // 4. Cột trụ điêu khắc trung tâm (Central Carved Pedestal)
    drawBox(0.0F, 0.55F, 0.0F, 0.65F, 1.10F, 0.65F, 0.75F, 0.73F, 0.68F);
    drawPyramidCap(0.0F, 1.10F, 0.0F, 0.80F, 0.15F, 0.80F, 0.78F, 0.75F, 0.70F);

    // 5. Đĩa hứng nước tầng 2 (Upper Basin Bowl)
    constexpr int upperSegments = 12;
    constexpr float upperRadius = 0.95F;
    constexpr float upperY = 1.35F;

    // Đĩa đá tầng 2
    for (int i = 0; i < upperSegments; ++i) {
        const float a1 = static_cast<float>(i) * (2.0F * PI / static_cast<float>(upperSegments));
        const float a2 = static_cast<float>(i + 1) * (2.0F * PI / static_cast<float>(upperSegments));
        const float x1 = std::cos(a1) * upperRadius, z1 = std::sin(a1) * upperRadius;
        const float x2 = std::cos(a2) * upperRadius, z2 = std::sin(a2) * upperRadius;
        const float mx = (x1 + x2) * 0.5F, mz = (z1 + z2) * 0.5F;
        const float len = std::sqrt((x2 - x1) * (x2 - x1) + (z2 - z1) * (z2 - z1));
        const float ang = std::atan2(x2 - x1, z2 - z1) * (180.0F / PI);

        glPushMatrix();
        glTranslatef(mx, upperY, mz);
        glRotatef(ang - 90.0F, 0.0F, 1.0F, 0.0F);
        drawBox(0.0F, 0.0F, 0.0F, len * 1.02F, 0.14F, 0.12F, 0.84F, 0.82F, 0.78F);
        glPopMatrix();
    }

    // Mặt nước tầng 2
    setMaterial(0.24F, 0.74F, 0.90F, 96.0F);
    glBegin(GL_POLYGON);
        glNormal3f(0.0F, 1.0F, 0.0F);
        for (int i = 0; i < upperSegments; ++i) {
            const float a = static_cast<float>(i) * (2.0F * PI / static_cast<float>(upperSegments));
            glVertex3f(std::cos(a) * (upperRadius - 0.05F), upperY + 0.05F, std::sin(a) * (upperRadius - 0.05F));
        }
    glEnd();

    // 6. Trụ phun đỉnh & vòi nước trung tâm (Top Spout)
    drawBox(0.0F, 1.60F, 0.0F, 0.32F, 0.50F, 0.32F, 0.78F, 0.76F, 0.72F);
    glPushMatrix();
    glTranslatef(0.0F, 1.90F, 0.0F);
    setMaterial(0.92F, 0.82F, 0.32F, 64.0F); // Đầu phun bằng đồng vàng
    glutSolidSphere(0.09, 12, 10);
    glPopMatrix();

    // 7. Cột nước phun vọt lên và tỏa hạt nước lấp lánh (Water Jets & Cascade Arches)
    // Cột nước chính vọt thẳng lên trời
    glPushMatrix();
    glTranslatef(0.0F, 1.96F, 0.0F);
    setMaterial(0.65F, 0.88F, 0.98F, 84.0F);
    // Chuỗi giọt nước phun lên cao
    glutSolidSphere(0.08, 10, 8);
    glTranslatef(0.0F, 0.18F, 0.0F);
    glutSolidSphere(0.07, 10, 8);
    glTranslatef(0.0F, 0.18F, 0.0F);
    glutSolidSphere(0.06, 10, 8);
    glTranslatef(0.0F, 0.16F, 0.0F);
    glutSolidSphere(0.045, 8, 6);
    glPopMatrix();

    // Các tia nước hình vòm tỏa tròn rơi xuống đĩa và hồ nước (Arching Water Jets)
    constexpr int jetCount = 8;
    for (int i = 0; i < jetCount; ++i) {
        const float a = static_cast<float>(i) * (2.0F * PI / static_cast<float>(jetCount));
        const float c = std::cos(a), s = std::sin(a);

        // Hạt nước vòm trên
        glPushMatrix();
        glTranslatef(c * 0.35F, 2.12F, s * 0.35F);
        setMaterial(0.70F, 0.90F, 1.0F, 96.0F);
        glutSolidSphere(0.045, 8, 6);
        glPopMatrix();

        // Hạt nước rơi xuống đĩa tầng 2
        glPushMatrix();
        glTranslatef(c * 0.68F, 1.65F, s * 0.68F);
        setMaterial(0.60F, 0.85F, 0.98F, 96.0F);
        glutSolidSphere(0.05, 8, 6);
        glPopMatrix();

        // Các tia nước tràn từ đĩa tầng 2 rơi xuống hồ tầng 1
        glPushMatrix();
        glTranslatef(c * 1.15F, 1.05F, s * 1.15F);
        setMaterial(0.55F, 0.82F, 0.96F, 96.0F);
        glutSolidSphere(0.055, 8, 6);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(c * 1.45F, 0.55F, s * 1.45F);
        setMaterial(0.50F, 0.80F, 0.95F, 96.0F);
        glutSolidSphere(0.05, 8, 6);
        glPopMatrix();
    }

    glPopMatrix();
}

}  // namespace

void drawGround() {
    // =========================================================================
    // 1. QUANG CẢNH MỞ RỘNG MÊNH MÔNG (VAST OPEN EXPANSIVE LANDSCAPE)
    // =========================================================================
    // Nền địa hình bãi cỏ bao la [-48, 48] x [-42, 42], hoàn toàn tự do thoáng đãng
    setMaterial(0.28F, 0.52F, 0.22F, 6.0F);
    glBegin(GL_QUADS);
        glNormal3f(0.0F, 1.0F, 0.0F);
        glVertex3f(-48.0F, -0.04F, -42.0F);
        glVertex3f( 48.0F, -0.04F, -42.0F);
        glVertex3f( 48.0F, -0.04F,  42.0F);
        glVertex3f(-48.0F, -0.04F,  42.0F);
    glEnd();

    // Thảm cỏ sân trước xanh ngọc tươi sáng, đón trọn ánh nắng mặt trời
    drawBox(0.0F, -0.015F, -6.5F, 28.0F, 0.03F, 16.0F, 0.32F, 0.58F, 0.24F);

    // =========================================================================
    // 2. KHU VỰC THỀM SÂN ĐÓN HIÊN NHÀ (PORCH PATIO LANDING)
    // =========================================================================
    drawBox(0.0F, 0.03F, -4.45F, 3.6F, 0.06F, 0.85F, 0.72F, 0.68F, 0.60F);
    drawBox(0.0F, 0.035F, -4.85F, 3.6F, 0.07F, 0.06F, 0.48F, 0.38F, 0.30F);

    // =========================================================================
    // 3. LỐI ĐI CHÍNH KẾT NỐI VÀO ĐÀI PHUN NƯỚC (WALKWAY TO FOUNTAIN PLAZA)
    // =========================================================================
    // Lối đi lát đá tự nhiên từ thềm nhà (-4.85) đến mép quảng trường đài phun nước (-7.5 + 3.6 = -3.9 -> Z ~ -4.85 đến -4.0)
    for (int i = 0; i < 4; ++i) {
        const float z = -4.95F - static_cast<float>(i) * 0.75F;
        // Phiến đá xám cát ấm
        drawBox(0.0F, 0.025F, z, 2.0F, 0.05F, 0.65F, 0.75F, 0.73F, 0.68F);
        // Gờ bó vỉa đá xám than hai bên lối đi
        drawBox(-1.06F, 0.035F, z, 0.12F, 0.07F, 0.72F, 0.45F, 0.43F, 0.40F);
        drawBox( 1.06F, 0.035F, z, 0.12F, 0.07F, 0.72F, 0.45F, 0.43F, 0.40F);
    }

    // =========================================================================
    // 4. ĐÀI PHUN NƯỚC TRUNG TÂM NGHỆ THUẬT (GRAND ARCHITECTURAL FOUNTAIN)
    // =========================================================================
    // Nằm ở trung tâm sân vườn mở: X = 0.0F, Z = -8.2F
    drawGrandFountain(0.0F, -8.2F);

    // Hai ghế băng sân vườn đặt đối xứng 2 bên quảng trường đài phun nước để ngắm cảnh
    drawGardenBench(-4.2F, -8.2F, 90.0F);  // Ghế bên trái hướng vào đài phun nước
    drawGardenBench( 4.2F, -8.2F, -90.0F); // Ghế bên phải hướng vào đài phun nước

    // Đèn lồng sân vườn trang nhã bốn phía quanh đài phun nước
    drawGardenLight(-2.8F, -5.6F);
    drawGardenLight( 2.8F, -5.6F);
    drawGardenLight(-2.8F, -10.8F);
    drawGardenLight( 2.8F, -10.8F);

    // =========================================================================
    // 5. ĐẠI VƯỜN HOA ĐA SẮC TỰ NHIÊN RỰC RỠ (EXTENSIVE BOTANICAL FLOWER GARDENS)
    // =========================================================================
    // --- Vườn hoa 1 (Trước nhà bên trái): Bản giao hưởng hoa Hồng & Đỏ Ruby ---
    drawBox(-5.2F, 0.10F, -4.6F, 3.2F, 0.20F, 1.8F, 0.50F, 0.35F, 0.22F);
    drawBox(-5.2F, 0.16F, -4.6F, 2.9F, 0.14F, 1.5F, 0.22F, 0.16F, 0.10F);
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 5; ++c) {
            const float fx = -6.2F + static_cast<float>(c) * 0.50F;
            const float fz = -5.1F + static_cast<float>(r) * 0.50F;
            if ((r + c) % 2 == 0) {
                drawFlower(fx, 0.17F, fz, 0.94F, 0.16F, 0.22F, 0.25F); // Đỏ thắm
            } else {
                drawFlower(fx, 0.17F, fz, 0.96F, 0.45F, 0.65F, 0.28F); // Hồng phấn hoa hồng
            }
        }
    }

    // --- Vườn hoa 2 (Trước nhà bên phải): Bản giao hưởng hoa Vàng Nắng & Cam Hoàng Hôn ---
    drawBox(5.2F, 0.10F, -4.6F, 3.2F, 0.20F, 1.8F, 0.50F, 0.35F, 0.22F);
    drawBox(5.2F, 0.16F, -4.6F, 2.9F, 0.14F, 1.5F, 0.22F, 0.16F, 0.10F);
    for (int r = 0; r < 3; ++r) {
        for (int c = 0; c < 5; ++c) {
            const float fx = 4.2F + static_cast<float>(c) * 0.50F;
            const float fz = -5.1F + static_cast<float>(r) * 0.50F;
            if ((r + c) % 2 == 0) {
                drawFlower(fx, 0.17F, fz, 0.98F, 0.84F, 0.15F, 0.28F); // Vàng rực
            } else {
                drawFlower(fx, 0.17F, fz, 0.96F, 0.50F, 0.16F, 0.24F); // Cam hoàng hôn
            }
        }
    }

    // --- Vườn hoa 3 (Vườn cánh cung bên trái đài phun nước): Tím Lavender & Xanh Thiên Thanh ---
    drawBox(-6.4F, 0.09F, -8.2F, 2.2F, 0.18F, 3.8F, 0.46F, 0.32F, 0.20F);
    drawBox(-6.4F, 0.15F, -8.2F, 1.9F, 0.12F, 3.5F, 0.22F, 0.16F, 0.10F);
    for (int r = 0; r < 6; ++r) {
        for (int c = 0; c < 3; ++c) {
            const float fx = -7.0F + static_cast<float>(c) * 0.60F;
            const float fz = -9.7F + static_cast<float>(r) * 0.60F;
            if ((r + c) % 3 == 0) {
                drawFlower(fx, 0.16F, fz, 0.68F, 0.35F, 0.88F, 0.26F); // Tím oải hương
            } else if ((r + c) % 3 == 1) {
                drawFlower(fx, 0.16F, fz, 0.35F, 0.60F, 0.92F, 0.28F); // Xanh thiên thanh
            } else {
                drawFlower(fx, 0.16F, fz, 0.85F, 0.45F, 0.92F, 0.24F); // Tím phong lan
            }
        }
    }

    // --- Vườn hoa 4 (Vườn cánh cung bên phải đài phun nước): Trắng Ngọc Trai & Hồng San Hô ---
    drawBox(6.4F, 0.09F, -8.2F, 2.2F, 0.18F, 3.8F, 0.46F, 0.32F, 0.20F);
    drawBox(6.4F, 0.15F, -8.2F, 1.9F, 0.12F, 3.5F, 0.22F, 0.16F, 0.10F);
    for (int r = 0; r < 6; ++r) {
        for (int c = 0; c < 3; ++c) {
            const float fx = 5.8F + static_cast<float>(c) * 0.60F;
            const float fz = -9.7F + static_cast<float>(r) * 0.60F;
            if ((r + c) % 2 == 0) {
                drawFlower(fx, 0.16F, fz, 0.96F, 0.96F, 0.92F, 0.26F); // Trắng thanh khiết
            } else {
                drawFlower(fx, 0.16F, fz, 0.96F, 0.42F, 0.38F, 0.28F); // Hồng san hô ấm
            }
        }
    }

    // --- Các bụi hoa cẩm tú cầu nở rộ & bụi cây cảnh lá đỏ tạo điểm nhấn cảnh quan ---
    drawHydrangeaBush(-3.1F, -3.8F, 0.46F, 0.60F, 0.42F, 0.85F); // Cẩm tú cầu tím biếc
    drawHydrangeaBush( 3.1F, -3.8F, 0.46F, 0.35F, 0.65F, 0.92F); // Cẩm tú cầu xanh biếc
    drawHydrangeaBush(-4.5F, -10.8F, 0.48F, 0.95F, 0.45F, 0.68F); // Cẩm tú cầu hồng thắm
    drawHydrangeaBush( 4.5F, -10.8F, 0.48F, 0.96F, 0.82F, 0.25F); // Cẩm tú cầu vàng kim

    // Bụi cây cảnh xén tỉa quanh nhà và đài phun nước
    drawShrub(-1.65F, 0.0F, -4.5F, 0.36F, 0.54F, 0.18F, 0.18F); // Bụi phong lá tía đỏ
    drawShrub( 1.65F, 0.0F, -4.5F, 0.36F, 0.16F, 0.44F, 0.16F); // Bụi xanh mướt
    drawShrub(-3.50F, 0.0F, -2.5F, 0.45F, 0.14F, 0.38F, 0.14F);
    drawShrub( 3.50F, 0.0F, -2.5F, 0.45F, 0.14F, 0.38F, 0.14F);

    // =========================================================================
    // 6. HOA DẠI TỰ NHIÊN RẢI RÁC TRÊN ĐỒNG CỎ BAO LA (WILDFLOWERS IN MEADOW)
    // =========================================================================
    const float meadowFlowers[][5] = {
        {-9.5F,  -5.5F, 0.96F, 0.96F, 0.92F},
        {-11.0F, -8.0F, 0.98F, 0.86F, 0.18F},
        {-8.5F, -12.5F, 0.45F, 0.68F, 0.95F},
        {-4.5F, -13.5F, 0.95F, 0.55F, 0.72F},
        { 0.0F, -13.8F, 0.98F, 0.86F, 0.18F},
        { 4.5F, -13.5F, 0.96F, 0.96F, 0.92F},
        { 8.5F, -12.5F, 0.45F, 0.68F, 0.95F},
        {11.0F,  -8.0F, 0.95F, 0.55F, 0.72F},
        { 9.5F,  -5.5F, 0.98F, 0.86F, 0.18F},
        {-12.5F, 2.0F,  0.96F, 0.96F, 0.92F},
        { 12.5F, 2.0F,  0.45F, 0.68F, 0.95F}
    };
    for (const auto& mf : meadowFlowers) {
        drawWildflower(mf[0], 0.0F, mf[1], mf[2], mf[3], mf[4]);
    }
}

void drawTree(float x, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, 0.0F, z);
    glScalef(scale, scale, scale);

    // Đế gốc cây bạnh rễ và đất màu mỡ
    drawBox(0.0F, 0.08F, 0.0F, 0.74F, 0.16F, 0.74F, 0.34F, 0.22F, 0.14F);

    // Thân cây gỗ sồi tự nhiên
    drawBox(0.0F, 0.85F, 0.0F, 0.46F, 1.50F, 0.46F, 0.38F, 0.22F, 0.11F);
    drawBox(0.0F, 1.95F, 0.0F, 0.38F, 0.90F, 0.38F, 0.40F, 0.24F, 0.12F);

    // Cành cây vươn ra nâng đỡ tán lá
    glPushMatrix();
    glTranslatef(-0.25F, 2.10F, 0.10F);
    glRotatef(35.0F, 0.0F, 0.0F, 1.0F);
    drawBox(0.0F, 0.0F, 0.0F, 0.18F, 0.65F, 0.18F, 0.36F, 0.20F, 0.10F);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.28F, 2.25F, -0.08F);
    glRotatef(-32.0F, 0.0F, 0.0F, 1.0F);
    drawBox(0.0F, 0.0F, 0.0F, 0.18F, 0.68F, 0.18F, 0.36F, 0.20F, 0.10F);
    glPopMatrix();

    // Tán lá cây bồng bềnh phân 3 tầng sắc độ xanh
    const float lowerFoliage[][4] = {
        {-0.72F, 2.75F,  0.15F, 0.92F},
        { 0.70F, 2.80F, -0.12F, 0.94F},
        { 0.05F, 2.70F, -0.70F, 0.88F},
        {-0.08F, 2.72F,  0.72F, 0.88F}
    };
    for (const auto& f : lowerFoliage) {
        glPushMatrix();
        glTranslatef(f[0], f[1], f[2]);
        setMaterial(0.14F, 0.38F, 0.15F, 8.0F);
        glutSolidSphere(f[3], 18, 14);
        glPopMatrix();
    }

    const float midFoliage[][4] = {
        { 0.0F,  3.35F,  0.0F,  1.12F},
        {-0.42F, 3.45F, -0.35F, 0.85F},
        { 0.45F, 3.50F,  0.32F, 0.85F}
    };
    for (const auto& f : midFoliage) {
        glPushMatrix();
        glTranslatef(f[0], f[1], f[2]);
        setMaterial(0.20F, 0.48F, 0.18F, 10.0F);
        glutSolidSphere(f[3], 18, 14);
        glPopMatrix();
    }

    // Tầng ngọn cây đón ánh nắng
    glPushMatrix();
    glTranslatef(0.0F, 4.20F, 0.0F);
    setMaterial(0.28F, 0.60F, 0.22F, 14.0F);
    glutSolidSphere(0.82, 16, 12);
    glPopMatrix();

    // Điểm xuyết quả mọng đỏ tươi
    const float berries[][3] = {
        {-0.65F, 3.10F,  0.75F},
        { 0.72F, 3.20F,  0.55F},
        {-0.45F, 3.65F, -0.65F},
        { 0.50F, 3.75F, -0.55F}
    };
    for (const auto& b : berries) {
        glPushMatrix();
        glTranslatef(b[0], b[1], b[2]);
        setMaterial(0.92F, 0.18F, 0.16F, 36.0F);
        glutSolidSphere(0.065, 8, 6);
        glPopMatrix();
    }

    glPopMatrix();
}

void drawFence() {
    // =========================================================================
    // HÀNG RÀO & CỔNG BIÊN MỞ RỘNG TỰ DO (OPEN EXPANSIVE BOUNDARY ACCENTS)
    // =========================================================================
    // Theo yêu cầu "quang cảnh rộng ra hết mức không bị rào gò bó":
    // Hàng rào không bóp nghẹt xung quanh ngôi nhà nữa mà được mở rộng thoáng đãng
    // ra xa ở đường biên khuôn viên (Z = -14.5F, X từ -15.0F đến 15.0F),
    // với cổng vòm rộng lớn mở toang chào đón vào quần thể sân vườn và đài phun nước!

    constexpr float zBoundary = -14.5F;
    constexpr float xLeftEdge = -14.0F;
    constexpr float xRightEdge = 14.0F;
    constexpr float gateOpeningLeft = -2.5F;
    constexpr float gateOpeningRight = 2.5F;

    // Màu sơn hàng rào gỗ trang nhã
    const float fenceR = 0.94F, fenceG = 0.92F, fenceB = 0.86F;
    const float railR = 0.82F, railG = 0.78F, railB = 0.70F;

    // 1. Hai dải hàng rào thấp trang trí mở rộng ra hai bên biên xa
    // Cánh trái biên xa (từ -14.0 đến -2.5)
    const float leftLen = gateOpeningLeft - xLeftEdge;
    const float leftMidX = (xLeftEdge + gateOpeningLeft) * 0.5F;
    drawBox(leftMidX, 0.35F, zBoundary, leftLen, 0.08F, 0.07F, railR, railG, railB);
    drawBox(leftMidX, 0.75F, zBoundary, leftLen, 0.08F, 0.07F, railR, railG, railB);
    constexpr int leftPickets = 12;
    for (int i = 0; i < leftPickets; ++i) {
        const float px = xLeftEdge + 0.3F + static_cast<float>(i) * ((leftLen - 0.6F) / (leftPickets - 1));
        drawBox(px, 0.45F, zBoundary, 0.10F, 0.85F, 0.04F, fenceR, fenceG, fenceB);
        drawPyramidCap(px, 0.87F, zBoundary, 0.10F, 0.06F, 0.04F, fenceR, fenceG, fenceB);
    }

    // Cánh phải biên xa (từ 2.5 đến 14.0)
    const float rightLen = xRightEdge - gateOpeningRight;
    const float rightMidX = (gateOpeningRight + xRightEdge) * 0.5F;
    drawBox(rightMidX, 0.35F, zBoundary, rightLen, 0.08F, 0.07F, railR, railG, railB);
    drawBox(rightMidX, 0.75F, zBoundary, rightLen, 0.08F, 0.07F, railR, railG, railB);
    constexpr int rightPickets = 12;
    for (int i = 0; i < rightPickets; ++i) {
        const float px = gateOpeningRight + 0.3F + static_cast<float>(i) * ((rightLen - 0.6F) / (rightPickets - 1));
        drawBox(px, 0.45F, zBoundary, 0.10F, 0.85F, 0.04F, fenceR, fenceG, fenceB);
        drawPyramidCap(px, 0.87F, zBoundary, 0.10F, 0.06F, 0.04F, fenceR, fenceG, fenceB);
    }

    // 2. Hai cột trụ cổng hoa trang nhã ở lối vào biên xa (X = ±2.5F, Z = -14.5F)
    const float pillarsX[2] = {gateOpeningLeft, gateOpeningRight};
    for (float px : pillarsX) {
        // Đế đá
        drawBox(px, 0.20F, zBoundary, 0.52F, 0.40F, 0.52F, 0.42F, 0.44F, 0.46F);
        // Thân trụ vữa kiến trúc sáng màu
        drawBox(px, 0.85F, zBoundary, 0.42F, 1.10F, 0.42F, 0.84F, 0.80F, 0.72F);
        // Mũ chóp nón
        drawPyramidCap(px, 1.40F, zBoundary, 0.52F, 0.20F, 0.52F, 0.45F, 0.40F, 0.35F);
        // Quả cầu trang trí ánh kim
        glPushMatrix();
        glTranslatef(px, 1.68F, zBoundary);
        setMaterial(0.95F, 0.82F, 0.30F, 64.0F);
        glutSolidSphere(0.09, 14, 10);
        glPopMatrix();
    }

    // 3. Hai cánh cổng mở rộng toang góc 55 độ sang 2 bên đón chào vào công viên
    // Cánh trái mở toang góc 55 độ
    glPushMatrix();
    glTranslatef(gateOpeningLeft, 0.0F, zBoundary);
    glRotatef(55.0F, 0.0F, 1.0F, 0.0F);
    drawBox(1.0F, 0.60F, 0.0F, 2.0F, 0.95F, 0.08F, 0.42F, 0.25F, 0.15F);
    drawBox(1.0F, 0.60F, 0.0F, 1.8F, 0.75F, 0.10F, 0.20F, 0.20F, 0.22F);
    glPopMatrix();

    // Cánh phải mở toang góc -55 độ
    glPushMatrix();
    glTranslatef(gateOpeningRight, 0.0F, zBoundary);
    glRotatef(-55.0F, 0.0F, 1.0F, 0.0F);
    drawBox(-1.0F, 0.60F, 0.0F, 2.0F, 0.95F, 0.08F, 0.42F, 0.25F, 0.15F);
    drawBox(-1.0F, 0.60F, 0.0F, 1.8F, 0.75F, 0.10F, 0.20F, 0.20F, 0.22F);
    glPopMatrix();
}

}  // namespace house
