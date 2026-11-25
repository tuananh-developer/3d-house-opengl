#include "house/scene.hpp"
#include "house/render_utils.hpp"

#if defined(__has_include)
  #if __has_include(<GL/freeglut.h>)
    #include <GL/freeglut.h>
  #elif __has_include(<GL/glut.h>)
    #include <GL/glut.h>
  #endif
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

namespace house {
namespace {

constexpr float kPi = 3.14159265358979323846F;

// ============================================================================
// 1. ROBUST MATERIAL & COLOR SYSTEM (Fixes grey wash-out from GL_COLOR_MATERIAL)
// ============================================================================

// Sets BOTH glColor3f AND glMaterialfv to guarantee vibrant colors regardless
// of whether GL_COLOR_MATERIAL is active, avoiding any grey tint leakage.
void applyMaterial(float r, float g, float b, float spec = 0.25F, float shininess = 32.0F) {
    glColor3f(r, g, b);
    const GLfloat diffuse[]  = {r, g, b, 1.0F};
    const GLfloat specular[] = {spec, spec, spec, 1.0F};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}

// Full 6-face box with explicit normals and guaranteed vibrant material color
void drawColoredBox(float x, float y, float z,
                    float width, float height, float depth,
                    float r, float g, float b,
                    float spec = 0.20F, float shininess = 24.0F) {
    glPushMatrix();
    glTranslatef(x, y, z);
    applyMaterial(r, g, b, spec, shininess);

    const float hx = width * 0.5F;
    const float hy = height * 0.5F;
    const float hz = depth * 0.5F;

    glBegin(GL_QUADS);
        // Front face (+Z)
        glNormal3f(0.0F, 0.0F, 1.0F);
        glVertex3f(-hx, -hy,  hz);
        glVertex3f( hx, -hy,  hz);
        glVertex3f( hx,  hy,  hz);
        glVertex3f(-hx,  hy,  hz);

        // Back face (-Z)
        glNormal3f(0.0F, 0.0F, -1.0F);
        glVertex3f(-hx, -hy, -hz);
        glVertex3f(-hx,  hy, -hz);
        glVertex3f( hx,  hy, -hz);
        glVertex3f( hx, -hy, -hz);

        // Top face (+Y)
        glNormal3f(0.0F, 1.0F, 0.0F);
        glVertex3f(-hx,  hy, -hz);
        glVertex3f(-hx,  hy,  hz);
        glVertex3f( hx,  hy,  hz);
        glVertex3f( hx,  hy, -hz);

        // Bottom face (-Y)
        glNormal3f(0.0F, -1.0F, 0.0F);
        glVertex3f(-hx, -hy, -hz);
        glVertex3f( hx, -hy, -hz);
        glVertex3f( hx, -hy,  hz);
        glVertex3f(-hx, -hy,  hz);

        // Right face (+X)
        glNormal3f(1.0F, 0.0F, 0.0F);
        glVertex3f( hx, -hy, -hz);
        glVertex3f( hx,  hy, -hz);
        glVertex3f( hx,  hy,  hz);
        glVertex3f( hx, -hy,  hz);

        // Left face (-X)
        glNormal3f(-1.0F, 0.0F, 0.0F);
        glVertex3f(-hx, -hy, -hz);
        glVertex3f(-hx, -hy,  hz);
        glVertex3f(-hx,  hy,  hz);
        glVertex3f(-hx,  hy, -hz);
    glEnd();

    glPopMatrix();
}

// Vertical solid cylinder with end disks
void drawColoredCylinder(float x, float y, float z,
                         float radius, float height,
                         float r, float g, float b,
                         float spec = 0.35F, float shininess = 40.0F,
                         int slices = 16) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(-90.0F, 1.0F, 0.0F, 0.0F);
    applyMaterial(r, g, b, spec, shininess);

    GLUquadric* quad = gluNewQuadric();
    if (quad) {
        gluQuadricNormals(quad, GLU_SMOOTH);
        gluCylinder(quad, radius, radius, height, slices, 1);

        // Bottom cap
        glPushMatrix();
        glRotatef(180.0F, 1.0F, 0.0F, 0.0F);
        gluDisk(quad, 0.0F, radius, slices, 1);
        glPopMatrix();

        // Top cap
        glPushMatrix();
        glTranslatef(0.0F, 0.0F, height);
        gluDisk(quad, 0.0F, radius, slices, 1);
        glPopMatrix();

        gluDeleteQuadric(quad);
    }
    glPopMatrix();
}

void drawColoredSphere(float x, float y, float z,
                       float radius,
                       float r, float g, float b,
                       float spec = 0.30F, float shininess = 32.0F,
                       int slices = 14, int stacks = 10) {
    glPushMatrix();
    glTranslatef(x, y, z);
    applyMaterial(r, g, b, spec, shininess);

    GLUquadric* quad = gluNewQuadric();
    if (quad) {
        gluQuadricNormals(quad, GLU_SMOOTH);
        gluSphere(quad, radius, slices, stacks);
        gluDeleteQuadric(quad);
    }
    glPopMatrix();
}

// ============================================================================
// 2. MODERN LUXURY COLOR PALETTE CONSTANTS
// ============================================================================

// Exterior Wall Finishes
constexpr float kWallWhiteR = 0.96F, kWallWhiteG = 0.94F, kWallWhiteB = 0.91F; // Warm White #F5F1E8
constexpr float kWallBeigeR = 0.79F, kWallBeigeG = 0.72F, kWallBeigeB = 0.62F; // Beige Accent #C9B99F
constexpr float kStoneTravR = 0.67F, kStoneTravG = 0.60F, kStoneTravB = 0.51F; // Travertine Stone #AA9881
constexpr float kStoneBaseR = 0.22F, kStoneBaseG = 0.24F, kStoneBaseB = 0.26F; // Dark Slate Plinth #383C42
constexpr float kParapetR   = 0.25F, kParapetG   = 0.27F, kParapetB   = 0.30F; // Dark Fascia Trim #41464D

// Wood & Metal Architectural Accents
constexpr float kWoodWalnutR = 0.44F, kWoodWalnutG = 0.28F, kWoodWalnutB = 0.17F; // Natural Walnut #70482D
constexpr float kWoodLightR  = 0.72F, kWoodLightG  = 0.54F, kWoodLightB  = 0.38F; // Light Scandinavian Oak #B78B61
constexpr float kGraphiteR   = 0.19F, kGraphiteG   = 0.22F, kGraphiteB   = 0.26F; // Dark Graphite Metal #303943
constexpr float kBronzeR     = 0.85F, kBronzeG     = 0.72F, kBronzeB     = 0.38F; // Brushed Bronze Accent

// Glass & Marble
constexpr float kGlassR      = 0.65F, kGlassG      = 0.78F, kGlassB      = 0.85F; // Light Sky Glass #A7C6D3
constexpr float kMarbleR     = 0.94F, kMarbleG     = 0.93F, kMarbleB     = 0.91F; // Calacatta White Marble

// Interior Floorings
constexpr float kFloorTileR  = 0.88F, kFloorTileG  = 0.86F, kFloorTileB  = 0.82F; // Ground Polished Porcelain
constexpr float kFloorWoodR  = 0.58F, kFloorWoodG  = 0.40F, kFloorWoodB  = 0.26F; // Upper Floors Hardwood

// ============================================================================
// 3. ARCHITECTURAL ASSEMBLY HELPERS (Windows, Doors, Railings, Stairs)
// ============================================================================

// High-end Architectural Window with frame, mullion, sill and glass pane
void drawArchitecturalWindow(float x, float y, float z,
                            float width, float height,
                            bool facingZ = true) {
    const float frameT = 0.06F;
    const float depth  = 0.10F;

    if (facingZ) {
        // Outer Graphite Window Frame
        drawColoredBox(x, y + height * 0.5F - frameT * 0.5F, z, width, frameT, depth, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);
        drawColoredBox(x, y - height * 0.5F + frameT * 0.5F, z, width, frameT, depth, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);
        drawColoredBox(x - width * 0.5F + frameT * 0.5F, y, z, frameT, height, depth, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);
        drawColoredBox(x + width * 0.5F - frameT * 0.5F, y, z, frameT, height, depth, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);

        // Center Mullion
        drawColoredBox(x, y, z, frameT * 0.7F, height - frameT * 2.0F, depth * 0.8F, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);

        // Projected Stone Window Sill at bottom
        drawColoredBox(x, y - height * 0.5F - 0.04F, z + 0.05F, width + 0.12F, 0.08F, depth + 0.10F, kStoneTravR, kStoneTravG, kStoneTravB);

        // Reflective Sky Glass Pane
        drawColoredBox(x, y, z, width - frameT * 2.0F, height - frameT * 2.0F, 0.02F, kGlassR, kGlassG, kGlassB, 0.75F, 80.0F);
    } else {
        // Aligned along X axis (facing Left/Right sides)
        drawColoredBox(x, y + height * 0.5F - frameT * 0.5F, z, depth, frameT, width, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);
        drawColoredBox(x, y - height * 0.5F + frameT * 0.5F, z, depth, frameT, width, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);
        drawColoredBox(x, y, z - width * 0.5F + frameT * 0.5F, depth, height, frameT, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);
        drawColoredBox(x, y, z + width * 0.5F - frameT * 0.5F, depth, height, frameT, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);

        // Center Mullion
        drawColoredBox(x, y, z, depth * 0.8F, height - frameT * 2.0F, frameT * 0.7F, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);

        // Projected Sill
        drawColoredBox(x + ((x > 0) ? 0.05F : -0.05F), y - height * 0.5F - 0.04F, z, depth + 0.10F, 0.08F, width + 0.12F, kStoneTravR, kStoneTravG, kStoneTravB);

        // Glass Pane
        drawColoredBox(x, y, z, 0.02F, height - frameT * 2.0F, width - frameT * 2.0F, kGlassR, kGlassG, kGlassB, 0.75F, 80.0F);
    }
}

// Architectural Doorway with Frame, Leaf, and Hardware
void drawArchitecturalDoor(float x, float y, float z,
                           float width, float height,
                           bool isGrandFrontDoor = false,
                           bool facingZ = true) {
    const float frameT = 0.06F;
    const float depth  = 0.12F;

    if (facingZ) {
        // Outer Door Frame
        drawColoredBox(x - width * 0.5F + frameT * 0.5F, y + height * 0.5F, z, frameT, height, depth, kGraphiteR, kGraphiteG, kGraphiteB);
        drawColoredBox(x + width * 0.5F - frameT * 0.5F, y + height * 0.5F, z, frameT, height, depth, kGraphiteR, kGraphiteG, kGraphiteB);
        drawColoredBox(x, y + height - frameT * 0.5F, z, width, frameT, depth, kGraphiteR, kGraphiteG, kGraphiteB);

        const float leafW = width - frameT * 2.0F;
        const float leafH = height - frameT;

        if (isGrandFrontDoor) {
            // Luxury Walnut Pivot Door with vertical fluting
            drawColoredBox(x, y + leafH * 0.5F, z, leafW, leafH, 0.06F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.35F);
            // Symmetrical decorative fluting panels
            drawColoredBox(x - leafW * 0.25F, y + leafH * 0.5F, z - 0.035F, leafW * 0.40F, leafH * 0.88F, 0.015F, 0.36F, 0.22F, 0.13F);
            drawColoredBox(x + leafW * 0.25F, y + leafH * 0.5F, z - 0.035F, leafW * 0.40F, leafH * 0.88F, 0.015F, 0.36F, 0.22F, 0.13F);
            // Long Architectural Bronze Pull Handle
            drawColoredCylinder(x + leafW * 0.35F, y + leafH * 0.18F, z - 0.06F, 0.018F, leafH * 0.65F, kBronzeR, kBronzeG, kBronzeB, 0.8F, 60.0F);
        } else {
            // Natural Walnut Interior Door
            drawColoredBox(x, y + leafH * 0.5F, z, leafW, leafH, 0.04F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.3F);
            // Bronze lever handle
            drawColoredBox(x + leafW * 0.35F, y + leafH * 0.48F, z - 0.035F, 0.12F, 0.03F, 0.04F, kBronzeR, kBronzeG, kBronzeB, 0.8F);
            drawColoredBox(x + leafW * 0.35F, y + leafH * 0.48F, z + 0.035F, 0.12F, 0.03F, 0.04F, kBronzeR, kBronzeG, kBronzeB, 0.8F);
        }
    } else {
        // Aligned along X axis
        drawColoredBox(x, y + height * 0.5F, z - width * 0.5F + frameT * 0.5F, depth, height, frameT, kGraphiteR, kGraphiteG, kGraphiteB);
        drawColoredBox(x, y + height * 0.5F, z + width * 0.5F - frameT * 0.5F, depth, height, frameT, kGraphiteR, kGraphiteG, kGraphiteB);
        drawColoredBox(x, y + height - frameT * 0.5F, z, depth, frameT, width, kGraphiteR, kGraphiteG, kGraphiteB);

        const float leafW = width - frameT * 2.0F;
        const float leafH = height - frameT;
        drawColoredBox(x, y + leafH * 0.5F, z, 0.04F, leafH, leafW, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.3F);
        drawColoredBox(x - 0.035F, y + leafH * 0.48F, z + leafW * 0.35F, 0.04F, 0.03F, 0.12F, kBronzeR, kBronzeG, kBronzeB, 0.8F);
        drawColoredBox(x + 0.035F, y + leafH * 0.48F, z + leafW * 0.35F, 0.04F, 0.03F, 0.12F, kBronzeR, kBronzeG, kBronzeB, 0.8F);
    }
}

// Modern Glass Railing with graphite posts and handrail
void drawBalconyRailing(float x1, float z1, float x2, float z2, float yBase, float height = 0.95F) {
    const float dx = x2 - x1;
    const float dz = z2 - z1;
    const float len = std::sqrt(dx * dx + dz * dz);
    if (len < 0.01F) return;

    const float angle = std::atan2(dz, dx) * 180.0F / kPi;
    const float midX  = (x1 + x2) * 0.5F;
    const float midZ  = (z1 + z2) * 0.5F;

    glPushMatrix();
    glTranslatef(midX, yBase, midZ);
    glRotatef(-angle, 0.0F, 1.0F, 0.0F);

    // Tempered Light Sky Glass Panel
    drawColoredBox(0.0F, height * 0.48F, 0.0F, len - 0.06F, height * 0.86F, 0.02F, kGlassR, kGlassG, kGlassB, 0.7F, 80.0F);
    // Dark Graphite Top Handrail
    drawColoredBox(0.0F, height, 0.0F, len, 0.05F, 0.06F, kGraphiteR, kGraphiteG, kGraphiteB, 0.5F);
    // Base Shoe Track
    drawColoredBox(0.0F, 0.03F, 0.0F, len, 0.05F, 0.06F, kGraphiteR, kGraphiteG, kGraphiteB, 0.4F);
    // Vertical Support Stanchions at ends
    drawColoredBox(-len * 0.48F, height * 0.5F, 0.0F, 0.04F, height, 0.04F, kGraphiteR, kGraphiteG, kGraphiteB, 0.5F);
    drawColoredBox( len * 0.48F, height * 0.5F, 0.0F, 0.04F, height, 0.04F, kGraphiteR, kGraphiteG, kGraphiteB, 0.5F);
    if (len > 2.0F) {
        drawColoredBox(0.0F, height * 0.5F, 0.0F, 0.04F, height, 0.04F, kGraphiteR, kGraphiteG, kGraphiteB, 0.5F);
    }
    glPopMatrix();
}

// Continuous Staircase with walnut treads, stone risers and glass railings
void drawLuxuryStaircase(float xMin, float xMax, float zMin, float zMax, float floorY, float height) {
    const float midZ = (zMin + zMax) * 0.5F;
    const float widthX = (xMax - xMin);
    const float flightW = widthX * 0.46F;
    const float landingY = floorY + height * 0.5F;
    const int stepsCount = 8;
    const float stepH = (height * 0.5F) / static_cast<float>(stepsCount);

    // Landing Slab at the back
    const float landingD = (zMax - midZ);
    drawColoredBox(xMin + widthX * 0.5F, landingY - 0.06F, midZ + landingD * 0.5F,
                   widthX, 0.12F, landingD, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.3F);

    // Flight 1: ascending from zMin to midZ
    const float flight1X = xMin + flightW * 0.5F;
    const float stepD = (midZ - zMin) / static_cast<float>(stepsCount);
    for (int i = 0; i < stepsCount; ++i) {
        const float cY = floorY + (static_cast<float>(i) + 0.5F) * stepH;
        const float cZ = zMin + (static_cast<float>(i) + 0.5F) * stepD;
        // Walnut Tread
        drawColoredBox(flight1X, cY, cZ, flightW, stepH * 0.92F, stepD * 1.05F,
                       kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.35F);
        // Travertine Riser Face
        drawColoredBox(flight1X, cY - stepH * 0.42F, cZ - stepD * 0.48F,
                       flightW * 0.98F, stepH * 0.25F, 0.02F, kStoneTravR, kStoneTravG, kStoneTravB);
    }

    // Flight 2: turning back from midZ to zMin
    const float flight2X = xMax - flightW * 0.5F;
    for (int i = 0; i < stepsCount; ++i) {
        const float cY = landingY + (static_cast<float>(i) + 0.5F) * stepH;
        const float cZ = midZ - (static_cast<float>(i) + 0.5F) * stepD;
        drawColoredBox(flight2X, cY, cZ, flightW, stepH * 0.92F, stepD * 1.05F,
                       kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.35F);
        drawColoredBox(flight2X, cY - stepH * 0.42F, cZ + stepD * 0.48F,
                       flightW * 0.98F, stepH * 0.25F, 0.02F, kStoneTravR, kStoneTravG, kStoneTravB);
    }

    // Safety Glass Guardrails
    drawBalconyRailing(xMin + widthX * 0.5F, zMin, xMin + widthX * 0.5F, midZ, floorY, 0.90F);
    drawBalconyRailing(xMin + widthX * 0.5F, zMin, xMin + widthX * 0.5F, midZ, landingY, 0.90F);
    drawBalconyRailing(xMin + 0.04F, zMin, xMin + 0.04F, zMax, floorY, 0.90F);
    drawBalconyRailing(xMax - 0.04F, zMin, xMax - 0.04F, zMax, landingY, 0.90F);
}

// ============================================================================
// 4. FURNITURE SYSTEM WITH VIBRANT NATURAL MATERIALS
// ============================================================================

void drawDesignerBed(float x, float y, float z, float width, float length, float rotY = 0.0F) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0.0F, 1.0F, 0.0F);

    // Walnut Bed Frame
    drawColoredBox(0.0F, 0.15F, 0.0F, width, 0.28F, length, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.3F);
    // Upholstered Headboard with Walnut border
    drawColoredBox(0.0F, 0.65F, -length * 0.48F, width * 1.05F, 0.92F, 0.10F, 0.32F, 0.20F, 0.12F);
    drawColoredBox(0.0F, 0.65F, -length * 0.46F, width * 0.95F, 0.82F, 0.06F, kWallBeigeR, kWallBeigeG, kWallBeigeB, 0.2F);

    // Warm White Plush Mattress
    drawColoredBox(0.0F, 0.36F, length * 0.04F, width * 0.92F, 0.26F, length * 0.88F, 0.96F, 0.95F, 0.93F, 0.1F);
    // Navy / Warm Grey Folded Duvet
    drawColoredBox(0.0F, 0.43F, length * 0.18F, width * 0.94F, 0.18F, length * 0.58F, 0.20F, 0.28F, 0.38F, 0.2F);

    // Soft Pillows
    const float pilW = width * 0.38F;
    drawColoredBox(-width * 0.24F, 0.52F, -length * 0.32F, pilW, 0.12F, 0.30F, 0.98F, 0.98F, 0.98F);
    drawColoredBox( width * 0.24F, 0.52F, -length * 0.32F, pilW, 0.12F, 0.30F, 0.98F, 0.98F, 0.98F);

    // Walnut Bedside Tables
    const float standDist = width * 0.62F;
    drawColoredBox(-standDist, 0.26F, -length * 0.42F, 0.42F, 0.50F, 0.42F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB);
    drawColoredBox( standDist, 0.26F, -length * 0.42F, 0.42F, 0.50F, 0.42F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB);

    // Bedside Table Lamps with Bronze Stem & Warm Shade
    drawColoredCylinder(-standDist, 0.51F, -length * 0.42F, 0.02F, 0.22F, kBronzeR, kBronzeG, kBronzeB, 0.8F);
    drawColoredBox(-standDist, 0.72F, -length * 0.42F, 0.20F, 0.18F, 0.20F, 0.98F, 0.92F, 0.78F);
    drawColoredCylinder( standDist, 0.51F, -length * 0.42F, 0.02F, 0.22F, kBronzeR, kBronzeG, kBronzeB, 0.8F);
    drawColoredBox( standDist, 0.72F, -length * 0.42F, 0.20F, 0.18F, 0.20F, 0.98F, 0.92F, 0.78F);

    glPopMatrix();
}

void drawDesignerWardrobe(float x, float y, float z, float width, float height, float depth, bool facingZ = true) {
    // Walnut Cabinet Box
    drawColoredBox(x, y + height * 0.5F, z, width, height, depth, 0.34F, 0.22F, 0.14F);

    const int doors = (width > 1.8F) ? 4 : 3;
    const float dW  = width / static_cast<float>(doors);
    for (int i = 0; i < doors; ++i) {
        const float cX = x - width * 0.5F + (static_cast<float>(i) + 0.5F) * dW;
        if (facingZ) {
            drawColoredBox(cX, y + height * 0.5F, z + depth * 0.51F, dW * 0.96F, height * 0.96F, 0.02F,
                           (i % 2 == 0) ? kWoodWalnutR : 0.40F, (i % 2 == 0) ? kWoodWalnutG : 0.25F, (i % 2 == 0) ? kWoodWalnutB : 0.15F);
            // Long Bronze Handle
            drawColoredBox(cX + dW * 0.38F, y + height * 0.48F, z + depth * 0.53F, 0.02F, 0.65F, 0.02F, kBronzeR, kBronzeG, kBronzeB, 0.8F);
        } else {
            drawColoredBox(x + depth * 0.51F, y + height * 0.5F, cX, 0.02F, height * 0.96F, dW * 0.96F,
                           (i % 2 == 0) ? kWoodWalnutR : 0.40F, (i % 2 == 0) ? kWoodWalnutG : 0.25F, (i % 2 == 0) ? kWoodWalnutB : 0.15F);
            drawColoredBox(x + depth * 0.53F, y + height * 0.48F, cX + dW * 0.38F, 0.02F, 0.65F, 0.02F, kBronzeR, kBronzeG, kBronzeB, 0.8F);
        }
    }
}

void drawBathroomSuite(float x, float y, float z, float w, float d) {
    // 1. Modern Porcelain Toilet
    const float tX = x - w * 0.30F;
    const float tZ = z + d * 0.28F;
    drawColoredBox(tX, y + 0.50F, tZ + 0.18F, 0.38F, 0.54F, 0.20F, 0.98F, 0.98F, 0.98F, 0.6F); // Tank
    drawColoredBox(tX, y + 0.24F, tZ - 0.04F, 0.36F, 0.42F, 0.44F, 0.98F, 0.98F, 0.98F, 0.6F); // Bowl

    // 2. Floating Marble Vanity Counter & LED Mirror
    const float vX = x + w * 0.22F;
    const float vZ = z + d * 0.28F;
    drawColoredBox(vX, y + 0.40F, vZ, 0.85F, 0.42F, 0.48F, kGraphiteR, kGraphiteG, kGraphiteB); // Cabinet
    drawColoredBox(vX, y + 0.63F, vZ, 0.90F, 0.06F, 0.52F, kMarbleR, kMarbleG, kMarbleB, 0.5F); // Marble Top
    drawColoredBox(vX, y + 0.68F, vZ, 0.52F, 0.08F, 0.36F, 0.98F, 0.98F, 0.98F, 0.7F); // Basin
    drawColoredCylinder(vX, y + 0.68F, vZ + 0.14F, 0.018F, 0.18F, 0.88F, 0.88F, 0.90F, 0.9F); // Chrome Tap
    drawColoredBox(vX, y + 1.35F, vZ + 0.22F, 0.84F, 0.98F, 0.02F, 0.98F, 0.92F, 0.75F); // LED backlight frame
    drawColoredBox(vX, y + 1.35F, vZ + 0.23F, 0.78F, 0.92F, 0.02F, kGlassR, kGlassG, kGlassB, 0.8F); // Mirror

    // 3. Walk-in Glass Shower Enclosure
    const float sX = x - w * 0.15F;
    const float sZ = z - d * 0.24F;
    drawColoredBox(sX, y + 0.03F, sZ, 0.95F, 0.05F, 0.95F, kFloorTileR, kFloorTileG, kFloorTileB); // Floor base
    drawColoredBox(sX + 0.47F, y + 1.10F, sZ, 0.02F, 2.10F, 0.95F, kGlassR, kGlassG, kGlassB, 0.7F, 80.0F); // Glass panel
    drawColoredBox(sX, y + 1.10F, sZ + 0.47F, 0.95F, 2.10F, 0.02F, kGlassR, kGlassG, kGlassB, 0.7F, 80.0F); // Glass door
    drawColoredCylinder(sX - 0.38F, y + 0.40F, sZ - 0.38F, 0.018F, 1.80F, 0.88F, 0.88F, 0.90F, 0.9F); // Chrome column
    drawColoredBox(sX - 0.12F, y + 2.15F, sZ - 0.38F, 0.18F, 0.02F, 0.18F, 0.88F, 0.88F, 0.90F, 0.9F); // Rain Head
}

void drawLushPottedPlant(float x, float y, float z, float scale = 1.0F) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scale, scale, scale);

    // Terracotta Plant Pot
    drawColoredCylinder(0.0F, 0.0F, 0.0F, 0.22F, 0.44F, 0.78F, 0.42F, 0.26F, 0.2F);
    // Soil
    drawColoredBox(0.0F, 0.42F, 0.0F, 0.40F, 0.02F, 0.40F, 0.22F, 0.15F, 0.09F);
    // Deep Green Lush Foliage
    drawColoredSphere(0.0F, 0.65F, 0.0F, 0.25F, 0.15F, 0.48F, 0.18F, 0.1F);
    drawColoredSphere(-0.10F, 0.80F, 0.06F, 0.20F, 0.20F, 0.58F, 0.22F, 0.1F);
    drawColoredSphere(0.12F, 0.75F, -0.08F, 0.21F, 0.18F, 0.52F, 0.20F, 0.1F);

    glPopMatrix();
}

// ============================================================================
// 5. REDESIGNED COMPLETE STRUCTURAL SYSTEM (Floors, Roof, Exterior & Interior Walls)
// ============================================================================

void drawArchitecturalFoundationAndFloors() {
    // Solid Dark Slate Plinth Base (Foundation Y = 0.0 to 0.35)
    drawColoredBox(0.0F, 0.175F, 1.0F, 9.60F, 0.35F, 8.40F, kStoneBaseR, kStoneBaseG, kStoneBaseB);

    // Front Grand Entrance Steps (Y = 0.0 to 0.35 at Z = -3.2 to -4.0)
    drawColoredBox(0.0F, 0.10F, -3.75F, 3.40F, 0.20F, 0.80F, kStoneTravR, kStoneTravG, kStoneTravB);
    drawColoredBox(0.0F, 0.23F, -3.40F, 3.00F, 0.22F, 0.65F, kStoneTravR, kStoneTravG, kStoneTravB);

    // Ground Floor Polished Tile Finish (Y = 0.355)
    drawColoredBox(0.0F, 0.355F, 1.0F, 9.40F, 0.01F, 8.20F, kFloorTileR, kFloorTileG, kFloorTileB, 0.4F);

    // Second Floor Hardwood Slab (Y = 3.0, thickness 0.22)
    // Left & Central slabs leaving openings for Stairwell [2.2, 4.4]x[0.1, 2.6]
    drawColoredBox(-2.9F, 2.89F, 2.10F, 3.60F, 0.22F, 5.80F, kFloorWoodR, kFloorWoodG, kFloorWoodB, 0.3F);
    drawColoredBox( 0.6F, 2.89F, 1.00F, 3.40F, 0.22F, 8.00F, kFloorWoodR, kFloorWoodG, kFloorWoodB, 0.3F);
    drawColoredBox( 3.5F, 2.89F, -1.60F, 2.40F, 0.22F, 3.20F, kFloorWoodR, kFloorWoodG, kFloorWoodB, 0.3F);
    drawColoredBox( 3.5F, 2.89F,  3.80F, 2.40F, 0.22F, 2.40F, kFloorWoodR, kFloorWoodG, kFloorWoodB, 0.3F);
    // Cantilevered Master Balcony Slab (Z = -3.0 to -3.8)
    drawColoredBox(-2.9F, 2.89F, -3.40F, 3.60F, 0.22F, 0.80F, kStoneTravR, kStoneTravG, kStoneTravB);

    // Third Floor Hardwood Slab (Y = 5.6, thickness 0.22)
    // Leaves openings for Stairwell
    drawColoredBox(-2.9F, 5.49F, 1.00F, 3.60F, 0.22F, 8.00F, kFloorWoodR, kFloorWoodG, kFloorWoodB, 0.3F);
    drawColoredBox( 0.8F, 5.49F, 3.00F, 3.80F, 0.22F, 4.00F, kFloorWoodR, kFloorWoodG, kFloorWoodB, 0.3F);
    drawColoredBox( 3.5F, 5.49F, -1.60F, 2.40F, 0.22F, 3.20F, kFloorWoodR, kFloorWoodG, kFloorWoodB, 0.3F);
    drawColoredBox( 3.5F, 5.49F,  3.80F, 2.40F, 0.22F, 2.40F, kFloorWoodR, kFloorWoodG, kFloorWoodB, 0.3F);

    // Roof Slab (Y = 8.1, thickness 0.22)
    drawColoredBox(0.0F, 8.11F, 1.00F, 9.60F, 0.22F, 8.20F, 0.85F, 0.84F, 0.82F);

    // Modern Parapet Walls with Dark Graphite Fascia Trim
    drawColoredBox( 0.0F, 8.35F, -3.05F, 9.60F, 0.38F, 0.24F, kParapetR, kParapetG, kParapetB);
    drawColoredBox( 0.0F, 8.35F,  5.05F, 9.60F, 0.38F, 0.24F, kParapetR, kParapetG, kParapetB);
    drawColoredBox(-4.75F, 8.35F, 1.00F, 0.24F, 0.38F, 8.20F, kParapetR, kParapetG, kParapetB);
    drawColoredBox( 4.75F, 8.35F, 1.00F, 0.24F, 0.38F, 8.20F, kParapetR, kParapetG, kParapetB);

    // Architectural Horizontal Fascia Bands (String Courses between floors)
    drawColoredBox(0.0F, 3.00F, -3.08F, 9.70F, 0.16F, 0.16F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox(0.0F, 5.60F, -3.08F, 9.70F, 0.16F, 0.16F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox(-4.85F, 3.00F, 1.00F, 0.16F, 0.16F, 8.30F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox( 4.85F, 3.00F, 1.00F, 0.16F, 0.16F, 8.30F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox(-4.85F, 5.60F, 1.00F, 0.16F, 0.16F, 8.30F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox( 4.85F, 5.60F, 1.00F, 0.16F, 0.16F, 8.30F, kGraphiteR, kGraphiteG, kGraphiteB);

    // Continuous Staircases
    drawLuxuryStaircase(2.2F, 4.4F, 0.1F, 2.5F, 0.35F, 2.65F);
    drawLuxuryStaircase(2.2F, 4.4F, 0.1F, 2.5F, 3.00F, 2.60F);
}

void drawCompleteExteriorEnvelope() {
    // ------------------------------------------------------------------------
    // MẶT TIỀN PHÍA TRƯỚC (FRONT ELEVATION, Z = -3.0)
    // ------------------------------------------------------------------------

    // TẦNG 1 MẶT TIỀN (Y: 0.35 -> 3.0)
    // Sảnh chính có mái che Portico (Cantilevered Canopy at Y = 3.08)
    drawColoredBox(0.0F, 3.08F, -3.65F, 3.20F, 0.18F, 1.50F, kStoneTravR, kStoneTravG, kStoneTravB);
    // Cột trụ sảnh ốp đá Travertine
    drawColoredBox(-1.45F, 1.68F, -4.20F, 0.30F, 2.65F, 0.30F, kStoneTravR, kStoneTravG, kStoneTravB);
    drawColoredBox( 1.45F, 1.68F, -4.20F, 0.30F, 2.65F, 0.30F, kStoneTravR, kStoneTravG, kStoneTravB);

    // Grand Front Door (Z = -3.0, X: [-0.7, 0.7])
    drawArchitecturalDoor(0.0F, 0.35F, -3.00F, 1.40F, 2.45F, true, true);
    // Vách đá trang trí cạnh cửa chính
    drawColoredBox(1.15F, 1.68F, -3.00F, 0.90F, 2.65F, 0.22F, kStoneTravR, kStoneTravG, kStoneTravB);

    // Living Room Floor-to-Ceiling Glass Wall (Left Wing)
    drawArchitecturalWindow(-3.00F, 1.68F, -3.00F, 2.80F, 2.45F, true);
    // Vertical Walnut Timber Louvers flanking living room
    for (int i = 0; i < 7; ++i) {
        drawColoredBox(-4.50F + static_cast<float>(i) * 0.18F, 1.68F, -3.05F, 0.05F, 2.65F, 0.08F,
                       kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.4F);
    }
    // Guest Bedroom Front Window (Right Wing)
    drawArchitecturalWindow(3.10F, 1.68F, -3.00F, 2.40F, 2.10F, true);
    // Solid Wall Segments filling the front facade
    drawColoredBox( 4.55F, 1.68F, -3.00F, 0.50F, 2.65F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox( 1.75F, 1.68F, -3.00F, 0.30F, 2.65F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);

    // TẦNG 2 MẶT TIỀN (Y: 3.0 -> 5.6)
    // Master Bedroom Sliding Glass Wall
    drawArchitecturalWindow(-2.90F, 4.30F, -3.00F, 3.20F, 2.40F, true);
    // Master Balcony Cantilevered Glass Railings
    drawBalconyRailing(-4.60F, -3.80F, -1.20F, -3.80F, 3.00F, 0.95F);
    drawBalconyRailing(-4.60F, -3.00F, -4.60F, -3.80F, 3.00F, 0.95F);
    drawBalconyRailing(-1.20F, -3.00F, -1.20F, -3.80F, 3.00F, 0.95F);
    drawLushPottedPlant(-4.20F, 3.00F, -3.45F, 0.85F);
    drawLushPottedPlant(-1.60F, 3.00F, -3.45F, 0.85F);

    // Center Architectural Travertine Feature Frame
    drawColoredBox(0.00F, 4.30F, -3.04F, 1.80F, 2.60F, 0.25F, kStoneTravR, kStoneTravG, kStoneTravB);
    // Bedroom 02 Front Window
    drawArchitecturalWindow(3.10F, 4.30F, -3.00F, 2.40F, 2.10F, true);
    drawColoredBox(4.55F, 4.30F, -3.00F, 0.50F, 2.60F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox(1.75F, 4.30F, -3.00F, 0.30F, 2.60F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);

    // TẦNG 3 MẶT TIỀN (Y: 5.6 -> 8.1)
    // Rooftop Terrace on Left Side: Open Terrace with Glass Railing & Modern Pergola
    drawBalconyRailing(-4.60F, -3.00F,  0.20F, -3.00F, 5.60F, 0.95F);
    drawBalconyRailing(-4.60F, -3.00F, -4.60F,  0.40F, 5.60F, 0.95F);
    drawBalconyRailing( 0.20F, -3.00F,  0.20F,  0.40F, 5.60F, 0.95F);

    // Modern Pergola on Terrace
    const float pergY = 5.60F + 2.40F;
    drawColoredCylinder(-4.40F, 5.60F, -2.80F, 0.04F, 2.40F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredCylinder(-0.20F, 5.60F, -2.80F, 0.04F, 2.40F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredCylinder(-4.40F, 5.60F,  0.20F, 0.04F, 2.40F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredCylinder(-0.20F, 5.60F,  0.20F, 0.04F, 2.40F, kGraphiteR, kGraphiteG, kGraphiteB);
    for (int p = 0; p < 7; ++p) {
        drawColoredBox(-2.30F, pergY, -2.70F + static_cast<float>(p) * 0.48F, 4.30F, 0.06F, 0.06F, kGraphiteR, kGraphiteG, kGraphiteB);
    }
    drawLushPottedPlant(-4.10F, 5.60F, -2.60F, 0.85F);
    drawLushPottedPlant(-2.30F, 5.60F, -2.60F, 0.85F);
    drawLushPottedPlant(-0.50F, 5.60F, -2.60F, 0.85F);

    // Bedroom 04 Front Window (Right Side)
    drawArchitecturalWindow(3.10F, 6.85F, -3.00F, 2.40F, 2.10F, true);
    drawColoredBox(4.55F, 6.85F, -3.00F, 0.50F, 2.50F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox(1.75F, 6.85F, -3.00F, 0.30F, 2.50F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);

    // ------------------------------------------------------------------------
    // HÔNG TRÁI (LEFT ELEVATION, X = -4.70)
    // ------------------------------------------------------------------------
    drawArchitecturalWindow(-4.70F, 1.68F, -0.6F, 2.40F, 2.20F, false);
    drawArchitecturalWindow(-4.70F, 1.68F,  2.8F, 2.20F, 1.80F, false);
    drawArchitecturalWindow(-4.70F, 4.30F,  1.5F, 2.60F, 2.10F, false);
    drawArchitecturalWindow(-4.70F, 6.85F,  2.2F, 2.80F, 2.10F, false);
    // Infill Solid Wall Panels
    drawColoredBox(-4.70F, 4.20F, -2.10F, 0.22F, 7.70F, 1.60F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox(-4.70F, 4.20F,  4.20F, 0.22F, 7.70F, 1.60F, kWallWhiteR, kWallWhiteG, kWallWhiteB);

    // ------------------------------------------------------------------------
    // HÔNG PHẢI (RIGHT ELEVATION, X = +4.70)
    // ------------------------------------------------------------------------
    drawArchitecturalWindow(4.70F, 1.68F, -1.6F, 1.80F, 1.80F, false);
    drawArchitecturalWindow(4.70F, 1.68F,  3.8F, 1.20F, 1.20F, false);
    drawArchitecturalWindow(4.70F, 4.30F, -1.6F, 1.80F, 1.80F, false);
    drawArchitecturalWindow(4.70F, 4.30F,  3.8F, 1.80F, 1.80F, false);
    drawArchitecturalWindow(4.70F, 6.85F, -1.6F, 1.80F, 1.80F, false);
    drawArchitecturalWindow(4.70F, 6.85F,  3.8F, 1.80F, 1.80F, false);
    // Infill Solid Wall Panels
    drawColoredBox(4.70F, 4.20F, 1.10F, 0.22F, 7.70F, 2.40F, kWallWhiteR, kWallWhiteG, kWallWhiteB);

    // ------------------------------------------------------------------------
    // PHÍA SAU (REAR ELEVATION, Z = +5.05)
    // ------------------------------------------------------------------------
    drawArchitecturalWindow(-3.30F, 1.68F, 5.05F, 2.20F, 2.30F, true);
    drawArchitecturalWindow( 0.40F, 1.68F, 5.05F, 2.60F, 2.30F, true);
    drawArchitecturalWindow(-2.00F, 4.30F, 5.05F, 1.80F, 1.60F, true);
    drawArchitecturalWindow( 3.10F, 4.30F, 5.05F, 2.20F, 2.10F, true);
    drawArchitecturalWindow(-0.40F, 6.85F, 5.05F, 1.80F, 1.80F, true);
    drawArchitecturalWindow( 3.10F, 6.85F, 5.05F, 2.20F, 1.80F, true);
    // Infill Solid Rear Wall Panels
    drawColoredBox(-4.20F, 4.20F, 5.05F, 1.20F, 7.70F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox( 1.80F, 4.20F, 5.05F, 0.80F, 7.70F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox( 4.50F, 4.20F, 5.05F, 0.60F, 7.70F, 0.22F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
}

// Complete Interior Partition Walls separating rooms and creating hallways
void drawCompleteInteriorPartitions() {
    // --- TẦNG 1 PARTITIONS (Y: 0.35 -> 3.0) ---
    // Wall separating Guest Bedroom (X > 1.6, Z < 0.0) with Doorway at X=1.65, Z=-0.2
    drawColoredBox(3.40F, 1.68F, 0.05F, 2.60F, 2.65F, 0.16F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawArchitecturalDoor(1.65F, 0.35F, -0.20F, 0.90F, 2.20F, false, true);

    // Wall separating Bathroom 01 & Laundry at back right (X > 1.6, Z > 3.2)
    drawColoredBox(3.40F, 1.68F, 3.20F, 2.60F, 2.65F, 0.16F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawArchitecturalDoor(2.10F, 0.35F, 3.20F, 0.85F, 2.20F, false, true);

    // Decorative Open Wood Slats dividing Foyer / Dining / Living Room
    for (int s = 0; s < 5; ++s) {
        drawColoredBox(-1.00F, 1.68F, -1.80F + static_cast<float>(s) * 0.25F, 0.04F, 2.65F, 0.08F,
                       kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.4F);
    }

    // --- TẦNG 2 PARTITIONS (Y: 3.0 -> 5.6) ---
    // Master Suite Private Wall & Door (X: -1.0 to -4.6, Z: 0.2 to 4.8)
    drawColoredBox(-1.00F, 4.30F, 1.20F, 0.16F, 2.60F, 2.20F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawArchitecturalDoor(-1.00F, 3.00F, 0.10F, 0.90F, 2.20F, false, false);
    // Master Bath / Closet Dividing Wall
    drawColoredBox(-2.50F, 4.30F, 3.80F, 0.16F, 2.60F, 2.20F, kWallWhiteR, kWallWhiteG, kWallWhiteB);

    // Bedroom 02 Partition (Front Right)
    drawColoredBox(3.40F, 4.30F, 0.05F, 2.60F, 2.60F, 0.16F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawArchitecturalDoor(1.65F, 3.00F, -0.20F, 0.90F, 2.20F, false, true);

    // Bedroom 03 Partition (Back Right)
    drawColoredBox(3.40F, 4.30F, 3.20F, 2.60F, 2.60F, 0.16F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawArchitecturalDoor(2.10F, 3.00F, 3.20F, 0.85F, 2.20F, false, true);

    // Shared Bathroom 02 Partition
    drawColoredBox(1.10F, 4.30F, 2.90F, 1.80F, 2.60F, 0.16F, kWallWhiteR, kWallWhiteG, kWallWhiteB);

    // --- TẦNG 3 PARTITIONS (Y: 5.6 -> 8.1) ---
    // Home Office Private Partition (Left Back)
    drawColoredBox(-1.10F, 6.85F, 2.60F, 0.16F, 2.50F, 3.60F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawArchitecturalDoor(-1.10F, 5.60F, 0.70F, 0.90F, 2.20F, false, false);

    // Bedroom 04 Partition (Front Right)
    drawColoredBox(3.40F, 6.85F, 0.05F, 2.60F, 2.50F, 0.16F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawArchitecturalDoor(1.65F, 5.60F, -0.20F, 0.90F, 2.20F, false, true);

    // Bedroom 05 Partition (Back Right)
    drawColoredBox(3.40F, 6.85F, 3.20F, 2.60F, 2.50F, 0.16F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawArchitecturalDoor(2.10F, 5.60F, 3.20F, 0.85F, 2.20F, false, true);

    // Bedroom 06 Partition (Back Center)
    drawColoredBox(-0.30F, 6.85F, 3.20F, 1.80F, 2.50F, 0.16F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
}

// ============================================================================
// 6. DETAILED ROOM-BY-ROOM INTERIORS
// ============================================================================

void drawGroundFloorRooms() {
    // --- 1. SẢNH ĐÓN (FOYER) ---
    drawColoredBox(-0.20F, 0.70F, -2.10F, 0.95F, 0.72F, 0.38F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB);
    drawColoredBox(-0.20F, 0.70F, -1.90F, 0.85F, 0.62F, 0.02F, kBronzeR, kBronzeG, kBronzeB, 0.7F);
    drawLushPottedPlant(0.85F, 0.35F, -2.20F, 0.90F);

    // --- 2. LUXURY LIVING ROOM (Phòng khách cánh trái) ---
    // L-Shaped Sectional Sofa (Cream White Upholstery with Warm Beige Cushions)
    drawColoredBox(-3.10F, 0.58F, -0.90F, 2.20F, 0.45F, 0.85F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox(-3.10F, 0.90F, -1.26F, 2.20F, 0.48F, 0.22F, 0.90F, 0.88F, 0.84F);
    drawColoredBox(-4.00F, 0.58F, -0.05F, 0.85F, 0.45F, 1.45F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox(-4.36F, 0.90F, -0.05F, 0.20F, 0.48F, 1.45F, 0.90F, 0.88F, 0.84F);
    // Decorative Accent Pillows (Olive Green & Terracotta)
    drawColoredBox(-2.50F, 0.85F, -1.10F, 0.38F, 0.32F, 0.12F, 0.75F, 0.42F, 0.25F);
    drawColoredBox(-3.40F, 0.85F, -1.10F, 0.38F, 0.32F, 0.12F, 0.35F, 0.48F, 0.30F);

    // Calacatta Marble Central Coffee Table
    drawColoredBox(-2.80F, 0.52F, 0.10F, 0.95F, 0.28F, 0.65F, kMarbleR, kMarbleG, kMarbleB, 0.6F, 60.0F);
    // Beige Plush Area Rug
    drawColoredBox(-2.90F, 0.36F, -0.20F, 2.70F, 0.02F, 2.30F, kWallBeigeR, kWallBeigeG, kWallBeigeB);

    // Modern TV Wall Unit: Calacatta Marble backdrop, Walnut floating credenza & 75" TV
    drawColoredBox(-2.90F, 1.70F, 1.12F, 2.80F, 2.20F, 0.05F, kMarbleR, kMarbleG, kMarbleB, 0.4F);
    drawColoredBox(-2.90F, 0.68F, 1.00F, 2.20F, 0.36F, 0.38F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB);
    drawColoredBox(-2.90F, 1.55F, 1.06F, 1.68F, 0.95F, 0.04F, 0.10F, 0.10F, 0.12F, 0.8F); // TV Frame
    drawColoredBox(-2.90F, 1.55F, 1.04F, 1.60F, 0.88F, 0.01F, 0.15F, 0.28F, 0.45F, 0.9F); // Screen

    // --- 3. BẾP HIỆN ĐẠI & ĐẢO BẾP (KITCHEN & ISLAND) ---
    // Countertop and Cabinets
    drawColoredBox(-3.30F, 0.80F, 4.55F, 2.50F, 0.90F, 0.65F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox(-3.30F, 1.26F, 4.55F, 2.54F, 0.05F, 0.68F, kMarbleR, kMarbleG, kMarbleB, 0.6F);
    drawColoredBox(-3.30F, 2.30F, 4.65F, 2.40F, 0.75F, 0.40F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    // Stainless Steel Refrigerator
    drawColoredBox(-1.65F, 1.45F, 4.50F, 0.85F, 2.20F, 0.75F, 0.85F, 0.86F, 0.88F, 0.7F);

    // Marble Island & Bar Stools
    drawColoredBox(-3.10F, 0.82F, 2.80F, 1.95F, 0.94F, 0.88F, kMarbleR, kMarbleG, kMarbleB, 0.6F);
    for (int s = 0; s < 2; ++s) {
        const float stX = -2.70F + static_cast<float>(s) * 0.75F;
        drawColoredCylinder(stX, 0.35F, 2.10F, 0.02F, 0.60F, kGraphiteR, kGraphiteG, kGraphiteB);
        drawColoredBox(stX, 0.96F, 2.10F, 0.38F, 0.08F, 0.38F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB);
    }

    // --- 4. PHÒNG ĂN (DINING ROOM) ---
    // Walnut 8-Seater Dining Table & Scandinavian Chairs
    drawColoredBox(0.40F, 0.74F, 3.20F, 1.90F, 0.06F, 0.98F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB, 0.4F);
    drawColoredBox(-0.35F, 0.37F, 3.20F, 0.06F, 0.70F, 0.75F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox( 1.15F, 0.37F, 3.20F, 0.06F, 0.70F, 0.75F, kGraphiteR, kGraphiteG, kGraphiteB);
    for (int i = 0; i < 3; ++i) {
        const float chX = -0.25F + static_cast<float>(i) * 0.65F;
        drawColoredBox(chX, 0.46F, 2.50F, 0.42F, 0.06F, 0.42F, kWallBeigeR, kWallBeigeG, kWallBeigeB);
        drawColoredBox(chX, 0.74F, 2.30F, 0.42F, 0.50F, 0.05F, kWallBeigeR, kWallBeigeG, kWallBeigeB);
        drawColoredBox(chX, 0.46F, 3.90F, 0.42F, 0.06F, 0.42F, kWallBeigeR, kWallBeigeG, kWallBeigeB);
        drawColoredBox(chX, 0.74F, 4.10F, 0.42F, 0.50F, 0.05F, kWallBeigeR, kWallBeigeG, kWallBeigeB);
    }
    // Modern Bronze Chandelier over dining table
    drawColoredBox(0.40F, 2.25F, 3.20F, 1.20F, 0.06F, 0.40F, kBronzeR, kBronzeG, kBronzeB, 0.8F);

    // --- 5. GUEST BEDROOM (BEDROOM 01 - TẦNG 1) ---
    drawDesignerBed(3.10F, 0.35F, -1.60F, 1.70F, 2.05F, 0.0F);
    drawDesignerWardrobe(4.25F, 0.35F, -1.60F, 1.80F, 2.45F, 0.55F, false);

    // --- 6. PHÒNG TẮM 01 & GIẶT (TẦNG 1) ---
    drawBathroomSuite(3.10F, 0.35F, 4.10F, 2.60F, 1.60F);
    // Washer & Dryer
    drawColoredBox(4.25F, 0.90F, 3.20F, 0.65F, 1.05F, 0.65F, 0.94F, 0.94F, 0.95F);
}

void drawSecondFloorRooms() {
    const float fY = 3.00F;

    // --- 1. MASTER BEDROOM SUITE ---
    drawDesignerBed(-2.80F, fY, 1.30F, 1.95F, 2.20F, 180.0F);
    // Relax Armchair by window
    drawColoredBox(-4.10F, fY + 0.44F, 2.30F, 0.65F, 0.42F, 0.65F, 0.35F, 0.45F, 0.55F);
    drawColoredBox(-4.10F, fY + 0.75F, 2.65F, 0.65F, 0.55F, 0.12F, 0.35F, 0.45F, 0.55F);

    // --- 2. WALK-IN CLOSET ---
    drawDesignerWardrobe(-3.40F, fY, 3.80F, 2.20F, 2.45F, 0.58F, true);
    // Illuminated Dressing Mirror
    drawColoredBox(-4.45F, fY + 1.20F, 3.80F, 0.02F, 1.80F, 0.75F, kGlassR, kGlassG, kGlassB, 0.8F);

    // --- 3. MASTER LUXURY BATHROOM (BATHROOM 02) ---
    // Freestanding Bathtub
    drawColoredBox(-1.60F, fY + 0.32F, 4.10F, 1.45F, 0.54F, 0.78F, 0.98F, 0.98F, 0.98F, 0.7F);
    drawColoredBox(-1.60F, fY + 0.38F, 4.10F, 1.25F, 0.45F, 0.58F, 0.75F, 0.88F, 0.95F, 0.5F);
    // Double Vanity Suite
    drawBathroomSuite(-1.50F, fY, 2.60F, 1.80F, 1.60F);

    // --- 4. FAMILY LOUNGE ---
    drawColoredBox(-0.60F, fY + 0.50F, -1.80F, 1.40F, 0.42F, 0.72F, kWallBeigeR, kWallBeigeG, kWallBeigeB);
    drawColoredBox(-0.60F, fY + 0.80F, -2.18F, 1.40F, 0.48F, 0.16F, 0.70F, 0.64F, 0.55F);
    drawColoredBox(-0.60F, fY + 0.38F, -1.10F, 0.75F, 0.24F, 0.45F, kMarbleR, kMarbleG, kMarbleB, 0.6F);

    // --- 5. BEDROOM 02 (Phía trước bên phải) ---
    drawDesignerBed(3.10F, fY, -1.60F, 1.70F, 2.05F, 0.0F);
    drawDesignerWardrobe(4.25F, fY, -1.60F, 1.70F, 2.45F, 0.55F, false);

    // --- 6. BEDROOM 03 (Phía sau bên phải) ---
    drawDesignerBed(3.10F, fY, 3.80F, 1.65F, 2.00F, 180.0F);
    drawDesignerWardrobe(4.25F, fY, 3.80F, 1.70F, 2.45F, 0.55F, false);

    // --- 7. SHARED BATHROOM 03 (TẦNG 2) ---
    drawBathroomSuite(1.00F, fY, 2.10F, 1.60F, 1.50F);
}

void drawThirdFloorRooms() {
    const float fY = 5.60F;

    // --- 1. HOME OFFICE / EXECUTIVE STUDY ---
    // Oak Desk with Dual Displays & Ergonomic Chair
    drawColoredBox(-2.80F, fY + 0.74F, 2.20F, 1.65F, 0.05F, 0.65F, kWoodLightR, kWoodLightG, kWoodLightB, 0.4F);
    drawColoredBox(-3.50F, fY + 0.36F, 2.20F, 0.04F, 0.72F, 0.58F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox(-2.10F, fY + 0.36F, 2.20F, 0.04F, 0.72F, 0.58F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox(-2.80F, fY + 1.05F, 2.05F, 0.85F, 0.44F, 0.03F, 0.10F, 0.10F, 0.12F, 0.8F); // Dual Monitors
    drawColoredBox(-2.80F, fY + 1.05F, 2.07F, 0.80F, 0.40F, 0.01F, 0.25F, 0.50F, 0.75F, 0.9F);
    // Bookshelf with colorful books
    drawColoredBox(-4.45F, fY + 1.15F, 2.20F, 0.38F, 2.30F, 1.80F, kWoodWalnutR, kWoodWalnutG, kWoodWalnutB);

    // --- 2. ENTERTAINMENT ROOM / HOME CINEMA ---
    drawColoredBox(0.40F, fY + 0.52F, 1.50F, 2.10F, 0.44F, 0.85F, 0.28F, 0.30F, 0.35F);
    drawColoredBox(0.40F, fY + 0.85F, 1.10F, 2.10F, 0.50F, 0.18F, 0.24F, 0.26F, 0.30F);
    drawColoredBox(0.40F, fY + 1.50F, 3.40F, 2.10F, 1.15F, 0.04F, 0.08F, 0.08F, 0.10F, 0.8F); // 85" Screen

    // --- 3. BEDROOM 04 (Phía trước bên phải) ---
    drawDesignerBed(3.10F, fY, -1.60F, 1.65F, 2.00F, 0.0F);
    drawDesignerWardrobe(4.25F, fY, -1.60F, 1.60F, 2.35F, 0.55F, false);

    // --- 4. BEDROOM 05 (Phía sau bên phải) ---
    drawDesignerBed(3.10F, fY, 4.00F, 1.65F, 2.00F, 180.0F);
    drawDesignerWardrobe(4.25F, fY, 4.00F, 1.60F, 2.35F, 0.55F, false);

    // --- 5. BEDROOM 06 (Phía sau trung tâm) ---
    drawDesignerBed(-0.40F, fY, 4.20F, 1.60F, 1.95F, 180.0F);
    drawDesignerWardrobe(-1.40F, fY, 4.20F, 1.40F, 2.35F, 0.52F, false);

    // --- 6. SHARED BATHROOM 04 (TẦNG 3) ---
    drawBathroomSuite(1.00F, fY, 2.40F, 1.50F, 1.40F);

    // --- 7. SÂN THƯỢNG (ROOFTOP TERRACE OUTDOOR FURNITURE) ---
    drawColoredBox(-2.50F, fY + 0.44F, -1.40F, 1.10F, 0.42F, 0.75F, kGraphiteR, kGraphiteG, kGraphiteB);
    drawColoredBox(-2.50F, fY + 0.40F, -0.65F, 0.65F, 0.38F, 0.55F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox(-2.50F, fY + 0.40F, -2.15F, 0.65F, 0.38F, 0.55F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox(-3.35F, fY + 0.40F, -1.40F, 0.55F, 0.38F, 0.65F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
    drawColoredBox(-1.65F, fY + 0.40F, -1.40F, 0.55F, 0.38F, 0.65F, kWallWhiteR, kWallWhiteG, kWallWhiteB);
}

}  // namespace

// ============================================================================
// 7. PUBLIC HOUSE DRAW ENTRY POINT (100% State-Isolated)
// ============================================================================

void drawHouse() {
    glPushMatrix();

    // 1. Foundation Plinth, Slabs, Fascia Bands, Staircases & Roof
    drawArchitecturalFoundationAndFloors();

    // 2. Complete Solid Exterior Wall Envelope (Front, Left, Right, Rear)
    drawCompleteExteriorEnvelope();

    // 3. Interior Walls creating individual private rooms and hallways
    drawCompleteInteriorPartitions();

    // 4. Fully-Furnished Vibrant Rooms across All 3 Floors:
    // Tầng 1: Foyer, Living Room, Kitchen, Dining Room, Bedroom 01, Bathroom 01
    drawGroundFloorRooms();

    // Tầng 2: Master Bedroom Suite, Walk-in Closet, Bathroom 02 (VIP),
    //         Bedroom 02, Bedroom 03, Bathroom 03, Family Lounge & Balcony
    drawSecondFloorRooms();

    // Tầng 3: Bedroom 04, Bedroom 05, Bedroom 06, Home Office, Cinema Room,
    //         Bathroom 04, Rooftop Terrace & Pergola
    drawThirdFloorRooms();

    glPopMatrix();
}

}  // namespace house
