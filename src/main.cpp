#include <GL/freeglut.h>
#include <GL/glu.h>

#include "house/camera.hpp"
#include "house/scene.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>

namespace {
constexpr int kInitialWindowWidth = 1280;
constexpr int kInitialWindowHeight = 800;
constexpr float kOrthographicHalfHeight = 11.5F;

enum class DragMode {
    None,
    Orbit,
    Pan
};

house::Camera camera;
int windowWidth = kInitialWindowWidth;
int windowHeight = kInitialWindowHeight;
bool perspectiveProjection = true;
bool lightingEnabled = true;
DragMode dragMode = DragMode::None;
int lastMouseX = 0;
int lastMouseY = 0;

void drawScene() {
    house::drawGround();
    house::drawHouse();
    house::drawTree(-7.2F, -1.2F, 1.05F);
    house::drawTree(7.4F, 1.0F, 1.15F);
    house::drawTree(-8.0F, 4.8F, 0.82F);
    house::drawFence();
}

void drawText(float x, float y, const std::string& text, void* font) {
    glRasterPos2f(x, y);
    for (const unsigned char character : text) {
        glutBitmapCharacter(font, static_cast<int>(character));
    }
}

void drawHud() {
    GLint previousMatrixMode = GL_MODELVIEW;
    glGetIntegerv(GL_MATRIX_MODE, &previousMatrixMode);

    // Isolate the HUD's temporary state changes from the 3D renderer.
    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_COLOR_BUFFER_BIT |
                 GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, static_cast<double>(windowWidth),
               0.0, static_cast<double>(windowHeight));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const float left = 18.0F;
    const float right = 530.0F;
    const float bottom = static_cast<float>(windowHeight) - 140.0F;
    const float top = static_cast<float>(windowHeight) - 18.0F;

    glColor4f(0.045F, 0.065F, 0.095F, 0.88F);
    glBegin(GL_QUADS);
        glVertex2f(left, bottom);
        glVertex2f(right, bottom);
        glVertex2f(right, top);
        glVertex2f(left, top);
    glEnd();

    glColor3f(0.96F, 0.94F, 0.88F);
    drawText(34.0F, static_cast<float>(windowHeight) - 45.0F,
             "3D HOUSE  |  OPENGL GRAPHICS DEMO", GLUT_BITMAP_HELVETICA_18);

    glColor3f(0.77F, 0.83F, 0.89F);
    drawText(34.0F, static_cast<float>(windowHeight) - 73.0F,
             "Left drag: orbit    Right drag: pan    Wheel: zoom",
             GLUT_BITMAP_HELVETICA_12);
    drawText(34.0F, static_cast<float>(windowHeight) - 94.0F,
             "P: projection    L: lighting    +/-: zoom    R: reset",
             GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.95F, 0.78F, 0.42F);
    drawText(34.0F, static_cast<float>(windowHeight) - 117.0F,
             std::string("Projection: ") +
                 (perspectiveProjection ? "Perspective" : "Orthographic") +
                 "    |    Lighting: " + (lightingEnabled ? "On" : "Off") +
                 "    |    Q / Esc: quit",
             GLUT_BITMAP_HELVETICA_12);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(previousMatrixMode);
    glPopAttrib();
}

void applyWorldLighting() {
    if (!lightingEnabled) {
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        return;
    }

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set the light after the view transform so its position remains fixed
    // in world space instead of following the camera.
    const GLfloat position[] = {-8.0F, 14.0F, 10.0F, 1.0F};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    const float aspect = static_cast<float>(windowWidth) /
                         static_cast<float>(std::max(windowHeight, 1));

    if (perspectiveProjection) {
        gluPerspective(48.0, aspect, 0.1, 120.0);
    } else {
        const double halfHeight = kOrthographicHalfHeight;
        const double halfWidth = halfHeight * static_cast<double>(aspect);
        glOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1, 120.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    camera.apply();
    applyWorldLighting();

    drawScene();
    drawHud();
    glutSwapBuffers();
}

void reshape(int width, int height) {
    windowWidth = std::max(width, 1);
    windowHeight = std::max(height, 1);
    glViewport(0, 0, windowWidth, windowHeight);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 27:
        case 'q':
        case 'Q':
            std::exit(EXIT_SUCCESS);

        case 'p':
        case 'P':
            perspectiveProjection = !perspectiveProjection;
            break;

        case 'l':
        case 'L':
            lightingEnabled = !lightingEnabled;
            break;

        case 'r':
        case 'R':
            camera.reset();
            perspectiveProjection = true;
            lightingEnabled = true;
            break;

        case '+':
        case '=':
            camera.zoom(-1.0F);
            break;

        case '-':
        case '_':
            camera.zoom(1.0F);
            break;

        default:
            break;
    }

    glutPostRedisplay();
}

void mouseButton(int button, int state, int x, int y) {
    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            dragMode = DragMode::Orbit;
        } else if (button == GLUT_RIGHT_BUTTON ||
                   button == GLUT_MIDDLE_BUTTON) {
            dragMode = DragMode::Pan;
        }

        lastMouseX = x;
        lastMouseY = y;
    } else if (state == GLUT_UP) {
        dragMode = DragMode::None;
    }

    glutPostRedisplay();
}

void mouseWheel(int, int direction, int, int) {
    camera.zoom(direction > 0 ? -1.0F : 1.0F);
    glutPostRedisplay();
}

void mouseMotion(int x, int y) {
    if (dragMode == DragMode::None) {
        return;
    }

    const float deltaX = static_cast<float>(x - lastMouseX);
    const float deltaY = static_cast<float>(y - lastMouseY);

    if (dragMode == DragMode::Orbit) {
        camera.orbit(deltaX, -deltaY);
    } else if (dragMode == DragMode::Pan) {
        camera.pan(deltaX, deltaY);
    }

    lastMouseX = x;
    lastMouseY = y;
    glutPostRedisplay();
}

void initializeOpenGL() {
    glClearColor(0.55F, 0.75F, 0.88F, 1.0F);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    const GLfloat ambient[] = {0.34F, 0.34F, 0.34F, 1.0F};
    const GLfloat diffuse[] = {0.95F, 0.92F, 0.84F, 1.0F};
    const GLfloat specular[] = {0.65F, 0.65F, 0.65F, 1.0F};

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

}  // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(kInitialWindowWidth, kInitialWindowHeight);
    glutInitWindowPosition(80, 60);
    glutCreateWindow("3D House - OpenGL / C++");

    initializeOpenGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMouseWheelFunc(mouseWheel);
    glutMotionFunc(mouseMotion);

    glutMainLoop();
    return EXIT_SUCCESS;
}
