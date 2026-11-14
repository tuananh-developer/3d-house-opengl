#include <GL/freeglut.h>
#include <GL/glu.h>
#include "house/camera.hpp"
#include "house/scene.hpp"
#include <algorithm>
#include <cstdlib>
#include <string>

namespace {
house::Camera camera;
int windowWidth = 1280, windowHeight = 800;
bool perspectiveProjection = true, lightingEnabled = true, dragging = false;
int lastMouseX = 0, lastMouseY = 0;

void drawScene() {
    house::drawGround();
    house::drawHouse();
    house::drawTree(-7.2F, -1.2F, 1.05F);
    house::drawTree(7.4F, 1.0F, 1.15F);
    house::drawTree(-8.0F, 4.8F, 0.82F);
    house::drawFence();
}
void drawText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (const unsigned char ch : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, ch);
}
void drawHud() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0.0, static_cast<double>(windowWidth), 0.0, static_cast<double>(windowHeight));
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);
    glColor3f(0.08F, 0.10F, 0.12F);
    glBegin(GL_QUADS);
        glVertex2f(16.0F, windowHeight - 112.0F); glVertex2f(370.0F, windowHeight - 112.0F);
        glVertex2f(370.0F, windowHeight - 16.0F); glVertex2f(16.0F, windowHeight - 16.0F);
    glEnd();
    glColor3f(0.96F, 0.94F, 0.88F);
    drawText(30.0F, windowHeight - 38.0F, "3D HOUSE - COMPUTER GRAPHICS");
    drawText(30.0F, windowHeight - 60.0F, "Drag mouse: orbit camera | Mouse wheel: zoom");
    drawText(30.0F, windowHeight - 80.0F, "P: projection | L: lighting | R: reset | ESC: exit");
    drawText(30.0F, windowHeight - 100.0F,
        std::string("Projection: ") + (perspectiveProjection ? "Perspective" : "Orthographic") +
        " | Lighting: " + (lightingEnabled ? "On" : "Off"));
    glEnable(GL_DEPTH_TEST);
    if (lightingEnabled) glEnable(GL_LIGHTING);
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    const float aspect = static_cast<float>(windowWidth) / static_cast<float>(std::max(windowHeight, 1));
    if (perspectiveProjection) gluPerspective(48.0, aspect, 0.1, 120.0);
    else {
        const double halfHeight = 11.5, halfWidth = halfHeight * static_cast<double>(aspect);
        glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1, 120.0);
    }
    glMatrixMode(GL_MODELVIEW); glLoadIdentity(); camera.apply();
    if (lightingEnabled) { glEnable(GL_LIGHTING); glEnable(GL_LIGHT0); }
    else glDisable(GL_LIGHTING);
    drawScene(); drawHud(); glutSwapBuffers();
}
void reshape(int width, int height) {
    windowWidth = std::max(width, 1); windowHeight = std::max(height, 1);
    glViewport(0, 0, windowWidth, windowHeight); glutPostRedisplay();
}
void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27: case 'q': case 'Q': std::exit(EXIT_SUCCESS);
        case 'p': case 'P': perspectiveProjection = !perspectiveProjection; break;
        case 'l': case 'L': lightingEnabled = !lightingEnabled; break;
        case 'r': case 'R': camera.reset(); perspectiveProjection = true; lightingEnabled = true; break;
        case '+': case '=': camera.zoom(-1.0F); break;
        case '-': case '_': camera.zoom(1.0F); break;
        default: break;
    }
    glutPostRedisplay();
}
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) { dragging = (state == GLUT_DOWN); lastMouseX = x; lastMouseY = y; }
#ifdef FREEGLUT
    if (state == GLUT_DOWN && button == 3) camera.zoom(-1.0F);
    else if (state == GLUT_DOWN && button == 4) camera.zoom(1.0F);
#endif
    glutPostRedisplay();
}
void mouseMotion(int x, int y) {
    if (!dragging) return;
    camera.orbit(static_cast<float>(x - lastMouseX), static_cast<float>(lastMouseY - y));
    lastMouseX = x; lastMouseY = y; glutPostRedisplay();
}
void initializeOpenGL() {
    glClearColor(0.55F, 0.75F, 0.88F, 1.0F);
    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL); glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE); glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    const GLfloat ambient[] = {0.34F, 0.34F, 0.34F, 1.0F};
    const GLfloat position[] = {-8.0F, 14.0F, 10.0F, 1.0F};
    const GLfloat diffuse[] = {0.95F, 0.92F, 0.84F, 1.0F};
    const GLfloat specular[] = {0.65F, 0.65F, 0.65F, 1.0F};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glEnable(GL_LIGHT0); glEnable(GL_LIGHTING);
}
}  // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(80, 60);
    glutCreateWindow("3D House - OpenGL / C++");
    initializeOpenGL();
    glutDisplayFunc(display); glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard); glutMouseFunc(mouseButton); glutMotionFunc(mouseMotion);
    glutMainLoop();
    return EXIT_SUCCESS;
}
