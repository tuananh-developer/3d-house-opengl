#include "house/scene.hpp"
#include "house/render_utils.hpp"
#include <GL/freeglut.h>
#include <cmath>

namespace house {
namespace {

constexpr float PI = 3.14159265358979323846F;

// Helper: Vẽ hình kim tự tháp 4 mặt làm chóp nón trang trí
void drawPyramidCap(float x, float y, float z, float width, float height, float depth,
                    float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    setMaterial(r, g, b, 32.0F);

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

// Helper: Vẽ đóa hoa nở rộ kích thước lớn (đường kính rõ nét nhìn từ xa)
void drawBigBloom(float x, float y, float z, float radius, float petalR, float petalG, float petalB,
                  float centerR = 0.98F, float centerG = 0.85F, float centerB = 0.15F) {
    // Thân cuống xanh
    drawBox(x, y + radius * 0.6F, z, 0.04F, radius * 1.2F, 0.04F, 0.16F, 0.52F, 0.16F);

    // Bông hoa chính nổi khối cầu rực rỡ
    glPushMatrix();
    glTranslatef(x, y + radius * 1.3F, z);
    setMaterial(petalR, petalG, petalB, 32.0F);
    glutSolidSphere(radius, 12, 10);

    // Nhụy hoa nổi bật tương phản
    glTranslatef(0.0F, radius * 0.35F, 0.0F);
    setMaterial(centerR, centerG, centerB, 48.0F);
    glutSolidSphere(radius * 0.42F, 10, 8);
    glPopMatrix();
}

// Helper: Vẽ thảm hoa lớn rực rỡ (Floral Color Bed) với màu nền rực rỡ và hàng loạt hoa nở rộ
void drawFlowerBed(float cx, float cz, float width, float depth,
                   float bedBorderR, float bedBorderG, float bedBorderB,
                   float flowerR, float flowerG, float flowerB,
                   float centerR, float centerG, float centerB,
                   int cols, int rows) {
    // 1. Khung bệ viền nổi bồn hoa
    drawBox(cx, 0.11F, cz, width, 0.22F, depth, bedBorderR, bedBorderG, bedBorderB);
    // 2. Thảm màu nền hoa tươi sáng
    drawBox(cx, 0.17F, cz, width - 0.20F, 0.14F, depth - 0.20F, flowerR * 0.85F, flowerG * 0.85F, flowerB * 0.85F);

    // 3. Các đóa hoa lớn nở rộ đều khắp bồn hoa
    const float stepX = (width - 0.50F) / static_cast<float>(std::max(cols - 1, 1));
    const float stepZ = (depth - 0.50F) / static_cast<float>(std::max(rows - 1, 1));
    const float startX = cx - (width - 0.50F) * 0.5F;
    const float startZ = cz - (depth - 0.50F) * 0.5F;

    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            const float fx = startX + static_cast<float>(c) * stepX;
            const float fz = startZ + static_cast<float>(r) * stepZ;
            drawBigBloom(fx, 0.18F, fz, 0.16F, flowerR, flowerG, flowerB, centerR, centerG, centerB);
        }
    }
}

// Helper: Vẽ chậu hoa đá lớn phong cách châu Âu (Grand Garden Urn)
void drawFloralUrn(float x, float z, float flowerR, float flowerG, float flowerB) {
    // Đế chậu đá cẩm thạch trắng
    drawBox(x, 0.22F, z, 0.50F, 0.44F, 0.50F, 0.88F, 0.86F, 0.82F);
    // Miệng chậu loe rộng
    drawBox(x, 0.48F, z, 0.62F, 0.14F, 0.62F, 0.92F, 0.90F, 0.85F);

    // Cụm hoa xum xuê nở tràn trên miệng chậu
    glPushMatrix();
    glTranslatef(x, 0.68F, z);
    setMaterial(flowerR, flowerG, flowerB, 28.0F);
    glutSolidSphere(0.32, 14, 12);
    glPopMatrix();

    // Các bông hoa điểm xuyết nhô ra
    const float offsets[][3] = {
        {-0.22F, 0.72F, 0.0F}, {0.22F, 0.72F, 0.0F},
        {0.0F, 0.72F, -0.22F}, {0.0F, 0.72F, 0.22F},
        {0.0F, 0.92F, 0.0F}
    };
    for (const auto& off : offsets) {
        glPushMatrix();
        glTranslatef(x + off[0], off[1], z + off[2]);
        setMaterial(0.98F, 0.88F, 0.20F, 36.0F);
        glutSolidSphere(0.11, 10, 8);
        glPopMatrix();
    }
}

// Helper: Vẽ bụi hoa cẩm tú cầu khổng lồ rực rỡ sắc màu
void drawBigHydrangeaBush(float x, float z, float radius, float flowerR, float flowerG, float flowerB) {
    // Bụi xanh nền tảng
    glPushMatrix();
    glTranslatef(x, radius * 0.70F, z);
    glScalef(1.0F, 0.85F, 1.0F);
    setMaterial(0.14F, 0.40F, 0.14F, 12.0F);
    glutSolidSphere(radius, 14, 10);
    glPopMatrix();

    // Các cụm hoa tròn to nở rực rỡ trên bề mặt
    const float clusters[][3] = {
        { 0.0F, radius * 1.35F, 0.0F},
        {-radius * 0.55F, radius * 1.10F,  radius * 0.45F},
        { radius * 0.55F, radius * 1.12F,  radius * 0.45F},
        {-radius * 0.50F, radius * 1.05F, -radius * 0.45F},
        { radius * 0.50F, radius * 1.08F, -radius * 0.45F}
    };
    for (const auto& c : clusters) {
        glPushMatrix();
        glTranslatef(x + c[0], c[1], z + c[2]);
        setMaterial(flowerR, flowerG, flowerB, 26.0F);
        glutSolidSphere(0.22, 12, 10);
        glPopMatrix();
    }
}

// Helper: Vẽ ghế gỗ công viên chân sắt đúc
void drawGardenBench(float x, float z, float angle) {
    glPushMatrix();
    glTranslatef(x, 0.0F, z);
    glRotatef(angle, 0.0F, 1.0F, 0.0F);

    // Chân sắt đúc đen
    const float ironR = 0.20F, ironG = 0.20F, ironB = 0.22F;
    drawBox(-0.65F, 0.24F, -0.16F, 0.06F, 0.48F, 0.06F, ironR, ironG, ironB);
    drawBox(-0.65F, 0.24F,  0.16F, 0.06F, 0.48F, 0.06F, ironR, ironG, ironB);
    drawBox( 0.65F, 0.24F, -0.16F, 0.06F, 0.48F, 0.06F, ironR, ironG, ironB);
    drawBox( 0.65F, 0.24F,  0.16F, 0.06F, 0.48F, 0.06F, ironR, ironG, ironB);

    // Nan gỗ màu nâu đỏ ấm áp
    const float woodR = 0.78F, woodG = 0.35F, woodB = 0.18F;
    drawBox(0.0F, 0.48F, -0.10F, 1.50F, 0.04F, 0.12F, woodR, woodG, woodB);
    drawBox(0.0F, 0.48F,  0.05F, 1.50F, 0.04F, 0.12F, woodR, woodG, woodB);
    drawBox(0.0F, 0.48F,  0.20F, 1.50F, 0.04F, 0.12F, woodR, woodG, woodB);
    drawBox(0.0F, 0.72F, 0.24F, 1.50F, 0.10F, 0.035F, woodR, woodG, woodB);
    drawBox(0.0F, 0.88F, 0.24F, 1.50F, 0.10F, 0.035F, woodR, woodG, woodB);

    glPopMatrix();
}

// Helper: Vẽ đèn lồng sân vườn trang nhã
void drawGardenLight(float x, float z) {
    drawBox(x, 0.25F, z, 0.10F, 0.50F, 0.10F, 0.18F, 0.18F, 0.22F);
    drawBox(x, 0.54F, z, 0.14F, 0.12F, 0.14F, 0.98F, 0.82F, 0.30F);
    drawBox(x, 0.62F, z, 0.18F, 0.04F, 0.18F, 0.22F, 0.22F, 0.25F);
    drawPyramidCap(x, 0.64F, z, 0.18F, 0.06F, 0.18F, 0.22F, 0.22F, 0.25F);
}

// =============================================================================
// ĐÀI PHUN NƯỚC NGHỆ THUẬT VỚI NƯỚC XANH NGỌC LAM RỰC RỠ & VIỀN VÀNG KIM
// =============================================================================
void drawGrandFountain(float centerX, float centerZ) {
    glPushMatrix();
    glTranslatef(centerX, 0.0F, centerZ);

    // 1. Đế móng quảng trường tròn lát đá hoa văn cát vàng ấm (Sandstone Plaza)
    constexpr int plazaSegments = 24;
    constexpr float plazaRadius = 3.6F;
    setMaterial(0.88F, 0.80F, 0.62F, 16.0F); // Đá cát vàng ấm áp
    glBegin(GL_POLYGON);
        glNormal3f(0.0F, 1.0F, 0.0F);
        for (int i = 0; i < plazaSegments; ++i) {
            const float a = static_cast<float>(i) * (2.0F * PI / static_cast<float>(plazaSegments));
            glVertex3f(std::cos(a) * plazaRadius, 0.02F, std::sin(a) * plazaRadius);
        }
    glEnd();

    // Vành viền đá gạch nung đỏ rực quanh quảng trường
    setMaterial(0.82F, 0.28F, 0.18F, 24.0F);
    glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= plazaSegments; ++i) {
            const float a = static_cast<float>(i % plazaSegments) * (2.0F * PI / static_cast<float>(plazaSegments));
            const float c = std::cos(a), s = std::sin(a);
            glNormal3f(c, 0.4F, s);
            glVertex3f(c * plazaRadius, 0.02F, s * plazaRadius);
            glVertex3f(c * (plazaRadius + 0.25F), 0.04F, s * (plazaRadius + 0.25F));
        }
    glEnd();

    // 2. Thành bể hồ nước lớn tầng 1 (Đá cẩm thạch trắng viền vàng kim)
    constexpr int poolSegments = 16;
    constexpr float poolRadius = 2.10F;
    constexpr float wallHeight = 0.50F;

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
        // Thành đá cẩm thạch trắng ngà
        drawBox(0.0F, 0.0F, 0.0F, len * 1.02F, wallHeight, 0.16F, 0.92F, 0.90F, 0.86F);
        // Viền phào chỉ vàng kim sáng rực trên thành hồ
        drawBox(0.0F, wallHeight * 0.5F + 0.02F, 0.0F, len * 1.05F, 0.05F, 0.22F, 0.98F, 0.82F, 0.22F);
        glPopMatrix();
    }

    // 3. Mặt nước xanh ngọc lam biển nhiệt đới cực kỳ rực rỡ (Vivid Cyan Tropical Water)
    setMaterial(0.0F, 0.85F, 1.0F, 96.0F);
    glBegin(GL_POLYGON);
        glNormal3f(0.0F, 1.0F, 0.0F);
        for (int i = 0; i < poolSegments; ++i) {
            const float a = static_cast<float>(i) * (2.0F * PI / static_cast<float>(poolSegments));
            glVertex3f(std::cos(a) * (poolRadius - 0.08F), wallHeight * 0.72F, std::sin(a) * (poolRadius - 0.08F));
        }
    glEnd();

    // 4. Cột trụ trung tâm bằng đá cẩm thạch
    drawBox(0.0F, 0.55F, 0.0F, 0.65F, 1.10F, 0.65F, 0.90F, 0.88F, 0.82F);
    drawPyramidCap(0.0F, 1.10F, 0.0F, 0.80F, 0.15F, 0.80F, 0.96F, 0.82F, 0.25F);

    // 5. Đĩa hứng nước tầng 2 viền vàng kim
    constexpr int upperSegments = 12;
    constexpr float upperRadius = 0.95F;
    constexpr float upperY = 1.35F;

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
        drawBox(0.0F, 0.0F, 0.0F, len * 1.02F, 0.14F, 0.12F, 0.96F, 0.82F, 0.22F);
        glPopMatrix();
    }

    // Mặt nước tầng 2 xanh ngọc sáng
    setMaterial(0.10F, 0.90F, 1.0F, 96.0F);
    glBegin(GL_POLYGON);
        glNormal3f(0.0F, 1.0F, 0.0F);
        for (int i = 0; i < upperSegments; ++i) {
            const float a = static_cast<float>(i) * (2.0F * PI / static_cast<float>(upperSegments));
            glVertex3f(std::cos(a) * (upperRadius - 0.05F), upperY + 0.05F, std::sin(a) * (upperRadius - 0.05F));
        }
    glEnd();

    // 6. Trụ phun đỉnh bằng đồng vàng sáng lóa
    drawBox(0.0F, 1.60F, 0.0F, 0.32F, 0.50F, 0.32F, 0.92F, 0.88F, 0.82F);
    glPushMatrix();
    glTranslatef(0.0F, 1.90F, 0.0F);
    setMaterial(0.98F, 0.85F, 0.20F, 72.0F);
    glutSolidSphere(0.10, 12, 10);
    glPopMatrix();

    // 7. Cột nước phun vọt lên và 8 tia nước vòm lấp lánh màu lam ngọc
    glPushMatrix();
    glTranslatef(0.0F, 1.96F, 0.0F);
    setMaterial(0.60F, 0.92F, 1.0F, 84.0F);
    glutSolidSphere(0.09, 10, 8);
    glTranslatef(0.0F, 0.20F, 0.0F);
    glutSolidSphere(0.08, 10, 8);
    glTranslatef(0.0F, 0.20F, 0.0F);
    glutSolidSphere(0.07, 10, 8);
    glTranslatef(0.0F, 0.18F, 0.0F);
    glutSolidSphere(0.05, 8, 6);
    glPopMatrix();

    // 8 tia nước hình vòm tỏa xuống tầng 2 và tầng 1
    constexpr int jetCount = 8;
    for (int i = 0; i < jetCount; ++i) {
        const float a = static_cast<float>(i) * (2.0F * PI / static_cast<float>(jetCount));
        const float c = std::cos(a), s = std::sin(a);

        glPushMatrix();
        glTranslatef(c * 0.35F, 2.15F, s * 0.35F);
        setMaterial(0.70F, 0.95F, 1.0F, 96.0F);
        glutSolidSphere(0.05, 8, 6);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(c * 0.68F, 1.68F, s * 0.68F);
        setMaterial(0.55F, 0.90F, 1.0F, 96.0F);
        glutSolidSphere(0.055, 8, 6);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(c * 1.15F, 1.08F, s * 1.15F);
        setMaterial(0.50F, 0.88F, 1.0F, 96.0F);
        glutSolidSphere(0.06, 8, 6);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(c * 1.45F, 0.58F, s * 1.45F);
        setMaterial(0.40F, 0.85F, 1.0F, 96.0F);
        glutSolidSphere(0.055, 8, 6);
        glPopMatrix();
    }

    glPopMatrix();
}

}  // namespace

void drawGround() {
    // =========================================================================
    // 1. QUANG CẢNH MỞ RỘNG MÊNH MÔNG (VAST EXPANSIVE GREEN MEADOW)
    // =========================================================================
    setMaterial(0.30F, 0.56F, 0.24F, 6.0F);
    glBegin(GL_QUADS);
        glNormal3f(0.0F, 1.0F, 0.0F);
        glVertex3f(-48.0F, -0.04F, -42.0F);
        glVertex3f( 48.0F, -0.04F, -42.0F);
        glVertex3f( 48.0F, -0.04F,  42.0F);
        glVertex3f(-48.0F, -0.04F,  42.0F);
    glEnd();

    // Thảm cỏ sân trước xanh tươi sáng đón nắng
    drawBox(0.0F, -0.015F, -6.5F, 28.0F, 0.03F, 16.0F, 0.35F, 0.62F, 0.26F);

    // =========================================================================
    // 2. KHU VỰC THỀM SÂN ĐÓN HIÊN NHÀ (PORCH PATIO LANDING)
    // =========================================================================
    // Thềm sân gạch màu đá cát vàng ấm áp
    drawBox(0.0F, 0.03F, -4.45F, 3.6F, 0.06F, 0.85F, 0.86F, 0.76F, 0.58F);
    // Viền thềm gạch nung đỏ rực
    drawBox(0.0F, 0.035F, -4.85F, 3.6F, 0.07F, 0.06F, 0.85F, 0.28F, 0.16F);

    // =========================================================================
    // 3. LỐI ĐI CHÍNH LÁT GẠCH NUNG ĐỎ ẤM & KHẢM ĐÁ CÁT VÀNG (WARM TERRACOTTA PATH)
    // =========================================================================
    for (int i = 0; i < 4; ++i) {
        const float z = -4.95F - static_cast<float>(i) * 0.75F;
        // Phiến gạch nung đỏ cam ấm áp
        drawBox(0.0F, 0.025F, z, 2.0F, 0.05F, 0.65F, 0.84F, 0.35F, 0.20F);
        // Gờ bó vỉa đá cát vàng kim hai bên lối đi
        drawBox(-1.06F, 0.035F, z, 0.12F, 0.07F, 0.72F, 0.95F, 0.82F, 0.30F);
        drawBox( 1.06F, 0.035F, z, 0.12F, 0.07F, 0.72F, 0.95F, 0.82F, 0.30F);
    }

    // =========================================================================
    // 4. ĐÀI PHUN NƯỚC NGHỆ THUẬT VÀ QUẢNG TRƯỜNG HOA
    // =========================================================================
    drawGrandFountain(0.0F, -8.2F);

    // Hai ghế băng sân vườn chân sắt đúc nan gỗ đỏ
    drawGardenBench(-4.2F, -8.2F, 90.0F);
    drawGardenBench( 4.2F, -8.2F, -90.0F);

    // Đèn lồng sân vườn bốn góc
    drawGardenLight(-2.8F, -5.6F);
    drawGardenLight( 2.8F, -5.6F);
    drawGardenLight(-2.8F, -10.8F);
    drawGardenLight( 2.8F, -10.8F);

    // 4 Chậu hoa đá lớn đặt tại 4 góc quảng trường đài phun nước rực rỡ sắc màu
    drawFloralUrn(-3.2F, -5.8F, 0.98F, 0.15F, 0.20F); // Chậu hoa Đỏ rực
    drawFloralUrn( 3.2F, -5.8F, 0.98F, 0.85F, 0.15F); // Chậu hoa Vàng rực
    drawFloralUrn(-3.2F, -10.6F, 0.88F, 0.18F, 0.90F); // Chậu hoa Tím phong lan
    drawFloralUrn( 3.2F, -10.6F, 0.0F, 0.75F, 1.0F);   // Chậu hoa Xanh lam ngọc

    // =========================================================================
    // 5. CÁC ĐẠI THẢM HOA ĐA SẮC RỰC RỠ KHỔNG LỒ (VIBRANT FLORAL COLOR BEDS)
    // =========================================================================
    // --- VƯỜN HOA 1 (Trước hiên trái): THẢM HOA ĐỎ RỰC (CRIMSON RED TULIPS) ---
    drawFlowerBed(-5.2F, -4.6F, 3.4F, 2.0F,
                  0.75F, 0.25F, 0.15F, // Viền gạch nung đỏ
                  0.98F, 0.10F, 0.18F, // Cánh hoa Đỏ rực chói lọi
                  0.98F, 0.90F, 0.15F, // Nhụy vàng rực
                  5, 3);

    // --- VƯỜN HOA 2 (Trước hiên phải): THẢM HOA VÀNG KIM MẶT TRỜI (GOLDEN SUNSHINE) ---
    drawFlowerBed(5.2F, -4.6F, 3.4F, 2.0F,
                  0.85F, 0.65F, 0.18F, // Viền đá vàng
                  0.98F, 0.88F, 0.08F, // Cánh hoa Vàng kim rực rỡ
                  0.98F, 0.40F, 0.10F, // Nhụy cam ấm
                  5, 3);

    // --- VƯỜN HOA 3 (Bên trái đài phun nước): THẢM HOA TÍM OẢI HƯƠNG & MAGENTA ---
    drawFlowerBed(-6.6F, -8.2F, 2.4F, 4.0F,
                  0.55F, 0.25F, 0.60F, // Viền tím trầm
                  0.88F, 0.15F, 0.95F, // Cánh hoa Tím Magenta biếc rực rỡ
                  0.98F, 0.90F, 0.20F, // Nhụy vàng kim
                  3, 6);

    // --- VƯỜN HOA 4 (Bên phải đài phun nước): THẢM HOA CAM RỰC LỬA & HỒNG SEN ---
    drawFlowerBed(6.6F, -8.2F, 2.4F, 4.0F,
                  0.85F, 0.35F, 0.18F, // Viền gạch cam
                  0.98F, 0.42F, 0.10F, // Cánh hoa Cam rực lửa
                  0.98F, 0.15F, 0.60F, // Nhụy hồng thắm
                  3, 6);

    // =========================================================================
    // 6. CÁC BỤI HOA CẨM TÚ CẦU KHỔNG LỒ RỰC RỠ SẮC MÀU
    // =========================================================================
    drawBigHydrangeaBush(-2.8F, -3.8F, 0.52F, 0.30F, 0.65F, 1.0F);  // Xanh dương ngọc biếc (Azure)
    drawBigHydrangeaBush( 2.8F, -3.8F, 0.52F, 0.98F, 0.25F, 0.68F); // Hồng cánh sen rực rỡ (Hot Pink)
    drawBigHydrangeaBush(-4.6F, -11.0F, 0.55F, 0.98F, 0.85F, 0.10F); // Vàng chanh rực sáng (Bright Lemon)
    drawBigHydrangeaBush( 4.6F, -11.0F, 0.55F, 0.75F, 0.20F, 0.95F); // Tím phong lan rực rỡ (Orchid Purple)

    // Các bụi cây cảnh lá đỏ phong (Crimson Topiary) cạnh thềm
    glPushMatrix();
    glTranslatef(-1.65F, 0.35F, -4.5F);
    setMaterial(0.85F, 0.15F, 0.18F, 16.0F); // Đỏ phong rực rỡ
    glutSolidSphere(0.42, 14, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(1.65F, 0.35F, -4.5F);
    setMaterial(0.98F, 0.55F, 0.15F, 16.0F); // Cam tươi rực rỡ
    glutSolidSphere(0.42, 14, 10);
    glPopMatrix();
}

// =============================================================================
// CÂY CỐI 3D ĐA SẮC MÀU (COLORFUL ORNAMENTAL TREES: RED MAPLE, SAKURA, GOLDEN BIRCH)
// =============================================================================
void drawTree(float x, float z, float scale) {
    glPushMatrix();
    glTranslatef(x, 0.0F, z);
    glScalef(scale, scale, scale);

    // Đế gốc cây và gờ đất sỏi màu nâu ấm
    drawBox(0.0F, 0.08F, 0.0F, 0.74F, 0.16F, 0.74F, 0.42F, 0.26F, 0.16F);

    // Thân cây gỗ sồi nâu ấm
    drawBox(0.0F, 0.85F, 0.0F, 0.46F, 1.50F, 0.46F, 0.42F, 0.24F, 0.12F);
    drawBox(0.0F, 1.95F, 0.0F, 0.38F, 0.90F, 0.38F, 0.45F, 0.26F, 0.14F);

    // Cành cây vươn ra
    glPushMatrix();
    glTranslatef(-0.25F, 2.10F, 0.10F);
    glRotatef(35.0F, 0.0F, 0.0F, 1.0F);
    drawBox(0.0F, 0.0F, 0.0F, 0.18F, 0.65F, 0.18F, 0.40F, 0.22F, 0.12F);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.28F, 2.25F, -0.08F);
    glRotatef(-32.0F, 0.0F, 0.0F, 1.0F);
    drawBox(0.0F, 0.0F, 0.0F, 0.18F, 0.68F, 0.18F, 0.40F, 0.22F, 0.12F);
    glPopMatrix();

    // DỰA VÀO TỌA ĐỘ X ĐỂ PHÂN BIỆT 3 GIỐNG CÂY MÀU SẮC RỰC RỠ:
    // Cây trái (X < -7.5): CÂY PHONG LÁ ĐỎ RỰC LỬA (RED AUTUMN MAPLE)
    // Cây phải (X > 7.0): CÂY HOA ANH ĐÀO HỒNG RỰC (PINK CHERRY BLOSSOM SAKURA)
    // Cây sau (X ~ -8.0): CÂY BẠCH DƯƠNG LÁ VÀNG KIM MÙA THU (GOLDEN BIRCH)
    float cr1, cg1, cb1; // Tầng tán dưới
    float cr2, cg2, cb2; // Tầng tán giữa
    float cr3, cg3, cb3; // Tầng tán trên

    if (x < -7.5F) {
        // Cây phong đỏ mùa thu (Red Maple)
        cr1 = 0.85F; cg1 = 0.12F; cb1 = 0.10F;
        cr2 = 0.95F; cg2 = 0.22F; cb2 = 0.12F;
        cr3 = 0.98F; cg3 = 0.45F; cb3 = 0.15F;
    } else if (x > 7.0F) {
        // Cây hoa anh đào nở rộ (Sakura Blossom)
        cr1 = 0.92F; cg1 = 0.42F; cb1 = 0.65F;
        cr2 = 0.98F; cg2 = 0.58F; cb2 = 0.78F;
        cr3 = 0.98F; cg3 = 0.78F; cb3 = 0.88F;
    } else {
        // Cây bạch dương lá vàng kim mùa thu (Golden Birch)
        cr1 = 0.85F; cg1 = 0.60F; cb1 = 0.10F;
        cr2 = 0.98F; cg2 = 0.82F; cb2 = 0.12F;
        cr3 = 0.98F; cg3 = 0.92F; cb3 = 0.25F;
    }

    // Tầng tán dưới
    const float lowerFoliage[][4] = {
        {-0.72F, 2.75F,  0.15F, 0.95F},
        { 0.70F, 2.80F, -0.12F, 0.96F},
        { 0.05F, 2.70F, -0.70F, 0.92F},
        {-0.08F, 2.72F,  0.72F, 0.92F}
    };
    for (const auto& f : lowerFoliage) {
        glPushMatrix();
        glTranslatef(f[0], f[1], f[2]);
        setMaterial(cr1, cg1, cb1, 10.0F);
        glutSolidSphere(f[3], 18, 14);
        glPopMatrix();
    }

    // Tầng tán giữa
    const float midFoliage[][4] = {
        { 0.0F,  3.35F,  0.0F,  1.15F},
        {-0.42F, 3.45F, -0.35F, 0.90F},
        { 0.45F, 3.50F,  0.32F, 0.90F}
    };
    for (const auto& f : midFoliage) {
        glPushMatrix();
        glTranslatef(f[0], f[1], f[2]);
        setMaterial(cr2, cg2, cb2, 12.0F);
        glutSolidSphere(f[3], 18, 14);
        glPopMatrix();
    }

    // Tầng ngọn cây rực rỡ đón nắng
    glPushMatrix();
    glTranslatef(0.0F, 4.20F, 0.0F);
    setMaterial(cr3, cg3, cb3, 16.0F);
    glutSolidSphere(0.88, 16, 12);
    glPopMatrix();

    glPopMatrix();
}

void drawFence() {
    // =========================================================================
    // HÀNG RÀO & CỔNG BIÊN MỞ RỘNG TỰ DO Ở BIÊN XA
    // =========================================================================
    constexpr float zBoundary = -14.5F;
    constexpr float xLeftEdge = -14.0F;
    constexpr float xRightEdge = 14.0F;
    constexpr float gateOpeningLeft = -2.5F;
    constexpr float gateOpeningRight = 2.5F;

    // Màu sơn hàng rào trắng kem ấm áp
    const float fenceR = 0.95F, fenceG = 0.93F, fenceB = 0.88F;
    const float railR = 0.85F, railG = 0.78F, railB = 0.68F;

    // Cánh trái biên xa
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

    // Cánh phải biên xa
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

    // Hai cột trụ cổng hoa trang nhã ở lối vào biên xa
    const float pillarsX[2] = {gateOpeningLeft, gateOpeningRight};
    for (float px : pillarsX) {
        drawBox(px, 0.20F, zBoundary, 0.52F, 0.40F, 0.52F, 0.45F, 0.46F, 0.48F);
        drawBox(px, 0.85F, zBoundary, 0.42F, 1.10F, 0.42F, 0.90F, 0.88F, 0.80F);
        drawPyramidCap(px, 1.40F, zBoundary, 0.52F, 0.20F, 0.52F, 0.98F, 0.82F, 0.25F);
        glPushMatrix();
        glTranslatef(px, 1.68F, zBoundary);
        setMaterial(0.98F, 0.85F, 0.25F, 64.0F);
        glutSolidSphere(0.10, 14, 10);
        glPopMatrix();
    }

    // Hai cánh cổng mở toang góc 55 độ đón khách
    glPushMatrix();
    glTranslatef(gateOpeningLeft, 0.0F, zBoundary);
    glRotatef(55.0F, 0.0F, 1.0F, 0.0F);
    drawBox(1.0F, 0.60F, 0.0F, 2.0F, 0.95F, 0.08F, 0.48F, 0.28F, 0.16F);
    drawBox(1.0F, 0.60F, 0.0F, 1.8F, 0.75F, 0.10F, 0.22F, 0.22F, 0.25F);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(gateOpeningRight, 0.0F, zBoundary);
    glRotatef(-55.0F, 0.0F, 1.0F, 0.0F);
    drawBox(-1.0F, 0.60F, 0.0F, 2.0F, 0.95F, 0.08F, 0.48F, 0.28F, 0.16F);
    drawBox(-1.0F, 0.60F, 0.0F, 1.8F, 0.75F, 0.10F, 0.22F, 0.22F, 0.25F);
    glPopMatrix();
}

}  // namespace house
