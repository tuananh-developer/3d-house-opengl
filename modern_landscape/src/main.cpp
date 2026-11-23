#include <glad/glad.h>
#include <iostream>
#include <memory>
#include <chrono>

#include "Camera.hpp"
#include "Scene.hpp"

// Cấu hình cửa sổ: Mặc định dùng GLFW (hoặc FreeGLUT nếu có flag -DUSE_GLUT)
#if defined(USE_GLUT)
#include <GL/freeglut.h>
#else
#define USE_GLFW 1
#include <GLFW/glfw3.h>
#endif

namespace {
int screenWidth = 1280;
int screenHeight = 800;

modern::Camera camera(glm::vec3(-2.0f, 7.0f, 27.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -14.0f);
std::unique_ptr<modern::Scene> scene;

float deltaTime = 0.0f;
#if defined(USE_GLFW)
float lastFrame = 0.0f;
#endif

bool firstMouse = true;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;

bool keys[1024] = {false};
}

#if defined(USE_GLFW)

void framebuffer_size_callback(GLFWwindow*, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow*, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Đảo ngược vì trục Y màn hình từ trên xuống dưới

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow*, double, double yoffset) {
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int, int action, int) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        camera.ToggleMode();
        std::cout << "[INFO] Camera Mode: " 
                  << (camera.Mode == modern::CameraMode::FIRST_PERSON ? "FIRST PERSON (WASD + Mouse)" : "ORBIT (Villa Focus)")
                  << std::endl;
    }

    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS)   keys[key] = true;
        else if (action == GLFW_RELEASE) keys[key] = false;
    }
}

void processInput(GLFWwindow*) {
    if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(modern::FORWARD, deltaTime);
    if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(modern::BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(modern::LEFT, deltaTime);
    if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(modern::RIGHT, deltaTime);
    if (keys[GLFW_KEY_SPACE]) camera.ProcessKeyboard(modern::UP, deltaTime);
    if (keys[GLFW_KEY_LEFT_SHIFT]) camera.ProcessKeyboard(modern::DOWN, deltaTime);
}

void savePPM(const char* filename, int width, int height) {
    std::vector<unsigned char> pixels(width * height * 3);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    FILE* f = fopen(filename, "wb");
    if (!f) return;
    fprintf(f, "P6\n%d %d\n255\n", width, height);
    for (int y = height - 1; y >= 0; --y) {
        fwrite(&pixels[y * width * 3], 1, width * 3, f);
    }
    fclose(f);
}

int runGLFW(const std::string& snapshotPath = "") {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Yêu cầu OpenGL 3.3 Core Profile (Tuyệt đối không dùng OpenGL cổ điển)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA khử răng cưa

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight,
        "Modern Landscape Architecture 3D - Raylib / Modern OpenGL", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // Khóa con trỏ chuột vào giữa cửa sổ để điều khiển góc nhìn FPS mượt mà
    if (snapshotPath.empty()) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "========================================================\n"
              << " Modern OpenGL 3.3+ Landscape Architecture Engine\n"
              << " GPU: " << glGetString(GL_RENDERER) << "\n"
              << " OpenGL Version: " << glGetString(GL_VERSION) << "\n"
              << " GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
              << "========================================================\n"
              << " Huong dan dieu khien:\n"
              << " - WASD: Di chuyen toi / lui / trai / phai\n"
              << " - Chuot: Xoay goc nhin 360 do (FPS Mouse Look)\n"
              << " - Space / Shift: Bay len cao / ha xuong thap\n"
              << " - Cuon chuot: Phong to / Thu nho (Zoom)\n"
              << " - Phim 'V': Chuyen doi che do First-Person <-> Orbit quanh biet thu\n"
              << " - Phim ESC: Thoat chuong trinh\n"
              << "========================================================\n";

    scene = std::make_unique<modern::Scene>();
    scene->init(screenWidth, screenHeight);

    if (!snapshotPath.empty()) {
        for (int frame = 0; frame < 5; ++frame) {
            scene->update(0.016f);
            scene->render(camera, screenWidth, screenHeight);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        savePPM(snapshotPath.c_str(), screenWidth, screenHeight);
        std::cout << "[INFO] Saved snapshot to " << snapshotPath << std::endl;
        scene.reset();
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        scene->update(deltaTime);
        scene->render(camera, screenWidth, screenHeight);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    scene.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

#else // USE_GLUT Fallback

void display() {
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
    lastTime = currentTime;

    // Di chuyển liên tục theo phím bấm
    if (keys['w'] || keys['W']) camera.ProcessKeyboard(modern::FORWARD, deltaTime);
    if (keys['s'] || keys['S']) camera.ProcessKeyboard(modern::BACKWARD, deltaTime);
    if (keys['a'] || keys['A']) camera.ProcessKeyboard(modern::LEFT, deltaTime);
    if (keys['d'] || keys['D']) camera.ProcessKeyboard(modern::RIGHT, deltaTime);
    if (keys[' '])              camera.ProcessKeyboard(modern::UP, deltaTime);
    if (keys['c'] || keys['C']) camera.ProcessKeyboard(modern::DOWN, deltaTime);

    scene->update(deltaTime);
    scene->render(camera, screenWidth, screenHeight);
    glutSwapBuffers();
    glutPostRedisplay();
}

void reshape(int w, int h) {
    screenWidth = std::max(w, 1);
    screenHeight = std::max(h, 1);
    glViewport(0, 0, screenWidth, screenHeight);
}

void motion(int x, int y) {
    if (firstMouse) {
        lastX = static_cast<float>(x);
        lastY = static_cast<float>(y);
        firstMouse = false;
    }
    float xoffset = static_cast<float>(x) - lastX;
    float yoffset = lastY - static_cast<float>(y);
    lastX = static_cast<float>(x);
    lastY = static_cast<float>(y);
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void keyboardDown(unsigned char key, int, int) {
    if (key == 27) std::exit(0); // ESC
    if (key == 'v' || key == 'V') camera.ToggleMode();
    keys[key] = true;
}

void keyboardUp(unsigned char key, int, int) {
    keys[key] = false;
}

int runGLUT(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitContextVersion(3, 3);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Modern Landscape Architecture 3D");

    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::cout << "========================================================\n"
              << " Modern OpenGL 3.3+ Core Profile Verified!\n"
              << " GPU: " << glGetString(GL_RENDERER) << "\n"
              << "========================================================\n";

    scene = std::make_unique<modern::Scene>();
    scene->init(screenWidth, screenHeight);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(motion);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    glutMainLoop();
    return 0;
}

#endif

int main(int argc, char** argv) {
#if defined(USE_GLUT)
    return runGLUT(argc, argv);
#else
    std::string snapshotPath = "";
    bool startOrbit = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--snapshot" && i + 1 < argc) {
            snapshotPath = argv[++i];
        } else if (std::string(argv[i]) == "--orbit") {
            startOrbit = true;
        }
    }
    if (startOrbit) {
        camera.Mode = modern::CameraMode::ORBIT;
        camera.OrbitRadius = 48.0f;
        camera.OrbitAzimuth = 25.0f;
        camera.OrbitElevation = 38.0f;
    }
    return runGLFW(snapshotPath);
#endif
}
