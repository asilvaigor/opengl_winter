//
// Created by igor on 22/04/2020.
//

#include "Scene.h"
#include "utils/SingletonException.h"

bool Scene::exists = false;
vcl::camera_scene Scene::camera;
vcl::camera_control_glfw Scene::cameraControl;
GLFWwindow *Scene::window;

Scene &Scene::getInstance(std::string windowTitle) {
    if (exists)
        throw SingletonException("Scene");
    exists = true;
    static Scene instance(windowTitle);
    return instance;
}

Scene::Scene(std::string &windowTitle) : windowTitle(windowTitle) {
    initializeInterface();
    setupScene();

    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    glfwSetScrollCallback(window, mouseScrollCallback);
    glfwSetKeyCallback(window, keyboardInputCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
}

Shaders &Scene::getShaders() {
    return *shaders;
}

void Scene::addObject(std::shared_ptr<Object> &object) {
    if (object->isMovable())
        movableObjects.push_back(object);
    else stillObjects.push_back(object);
}

void Scene::display() {
    // Preprocessing the depth map for objects that do not move
    updateDepthMap(stillObjects);

    while (!glfwWindowShouldClose(window)) {
        // Clears screen, updates gui and objects
        opengl_debug();
        clearScreen();
        opengl_debug();
        updateGui();
        updateScene();
        opengl_debug();

        // Render gui and update window
        ImGui::End();
        cameraControl.update = !(ImGui::IsAnyWindowFocused());
        vcl::imgui_render_frame(window);
        updateFps();
        glfwSwapBuffers(window);
        glfwPollEvents();
        opengl_debug();
    }

    vcl::imgui_cleanup();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Scene::clearScreen() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    whiteTexture->bind();
}

void Scene::initializeInterface() {
    vcl::glfw_init();

    const int openglVersionMajor = 3;
    const int openglVersionMinor = 3;
    windowWidth = 1280;
    windowHeight = 1000;
    window = vcl::glfw_create_window(windowWidth, windowHeight, windowTitle, openglVersionMajor,
                                     openglVersionMinor);

    vcl::glad_init();
    vcl::opengl_debug_print_version();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    vcl::imgui_init(window);
}

void Scene::setupScene() {
    shaders = Shaders::getInstance();
    showWireframe = false;

    // Setting camera perspective
    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);
    const float aspect_ratio = (float) width / (float) height;
    camera.perspective = vcl::perspective_structure(
            40 * 3.14f / 180, aspect_ratio, 0.01f, 500.0f);

    // Setting initial camera position
    camera.apply_scaling(3);
    camera.apply_rotation(0, 0, 0, 0.8);

    // Creating rendering stuff
    whiteTexture = std::make_shared<Texture>();
    light = vcl::light_source({10, 0, 3});
    depthMap = std::make_shared<vcl::depth_map>(2048);
}

void Scene::updateFps() {
    if (fpsCounter.update()) {
        const std::string new_window_title = windowTitle + " (" + std::to_string(fpsCounter.fps()) + " fps)";
        glfwSetWindowTitle(window, new_window_title.c_str());
        fpsCounter.reset();
    }
}

void Scene::updateGui() {
    vcl::imgui_create_frame();
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    ImGui::Checkbox("Wireframe", &showWireframe);
    shaders->overrideWithWireframe(showWireframe);
}

void Scene::updateDepthMap(std::vector<std::shared_ptr<Object>> &objects) {
    depthMap->bind();
    shaders->overrideWithDepth(true);
    for (auto &obj : objects) {
        obj->draw(camera, light);
        whiteTexture->bind();
    }
    shaders->overrideWithDepth(false);
    depthMap->unbind(windowWidth, windowHeight);
}

void Scene::updateScene() {
    // TODO
//    updateDepthMap(movableObjects);
    updateDepthMap(stillObjects);

    for (auto
    &obj : stillObjects) {
        obj->draw(camera, light);
        whiteTexture->bind();
    }
    for (auto &obj : movableObjects) {
        obj->draw(camera, light);
        whiteTexture->bind();
    }
}

void Scene::windowSizeCallback(GLFWwindow *, int width, int height) {
    glViewport(0, 0, width, height);
    camera.perspective.image_aspect = (float) width / (float) height;
}

void Scene::cursorPositionCallback(GLFWwindow *w, double xpos, double ypos) {
    cameraControl.update_mouse_move(camera, w, float(xpos), float(ypos));
}

void Scene::mouseClickCallback(GLFWwindow *, int button, int action, int mods) {
    ImGui::SetWindowFocus(nullptr);
    cameraControl.update_mouse_click(camera, window, button, action, mods);
}

void Scene::mouseScrollCallback(GLFWwindow *, double xoffset, double yoffset) {
    cameraControl.update_mouse_scroll(camera, window, xoffset, yoffset);
}

void Scene::keyboardInputCallback(GLFWwindow *, int, int, int, int) {

}
