#ifndef APP_H
#define APP_H

#include <GLFW/glfw3.h>
#include "Library.h"

class App {
public:
    App();
    ~App();
    void run();

private:
    void initWindow();
    void initImGui();
    void processEvents();
    void update();
    void render();
    void cleanup();

    GLFWwindow* window;
    Library library;
    
    // Window settings
    const int windowWidth = 1000;
    const int windowHeight = 700;
};

#endif