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
    
    // Helper for custom styles
    void applyTheme();

    GLFWwindow* window;
    Library library;
    
    // Window settings - Increased default size
    const int windowWidth = 1280;
    const int windowHeight = 720;
};

#endif