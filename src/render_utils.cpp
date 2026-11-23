#include "house/render_utils.hpp"
#include <GL/freeglut.h>

namespace house {
void setMaterial(float r, float g, float b, float shininess) {
    const GLfloat color[] = {r, g, b, 1.0F};
    const GLfloat specular[] = {0.22F, 0.22F, 0.22F, 1.0F};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    // Bắt buộc gọi glColor3f để đồng bộ với GL_COLOR_MATERIAL (tránh bị HUD ghi đè màu trắng)
    glColor3f(r, g, b);
}
void drawBox(float x, float y, float z, float width, float height, float depth,
             float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(width, height, depth);
    setMaterial(r, g, b);
    glutSolidCube(1.0);
    glPopMatrix();
}
}  // namespace house
