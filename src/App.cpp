#include "App.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstring>
#include <stdexcept>
#include <iostream>

App::App() : window(nullptr)
{
    initWindow();
    initImGui();
}

App::~App()
{
    cleanup();
}

void App::initWindow()
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(windowWidth, windowHeight, "Library Management System", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
}

void App::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void App::run()
{
    while (!glfwWindowShouldClose(window))
    {
        processEvents();
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        update();
        render();
    }
}

void App::processEvents()
{
    glfwPollEvents();
}

void App::update()
{
    // Input buffers
    static char titleBuf[128] = "";
    static char authorBuf[128] = "";
    static int quantity = 1;
    static int bookId = 1;
    static int searchId = 0;

    ImGui::Begin("Library Management System", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    // Add Book Section
    ImGui::SeparatorText("Add New Book");
    ImGui::InputInt("Book ID", &bookId);
    ImGui::InputText("Title", titleBuf, IM_ARRAYSIZE(titleBuf));
    ImGui::InputText("Author", authorBuf, IM_ARRAYSIZE(authorBuf));
    ImGui::InputInt("Quantity", &quantity);
    
    if (ImGui::Button("Add Book")) {
        if (strlen(titleBuf) > 0 && strlen(authorBuf) > 0) {
            library.addBook({bookId, std::string(titleBuf), std::string(authorBuf), quantity});
            bookId++;
            titleBuf[0] = '\0';
            authorBuf[0] = '\0';
            quantity = 1;
        }
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Search/Remove Section
    ImGui::SeparatorText("Search & Remove");
    ImGui::InputInt("Search ID", &searchId);
    ImGui::SameLine();
    
    if (ImGui::Button("Find Book")) {
        Book* found = library.findBook(searchId);
        if (found) {
            ImGui::OpenPopup("Book Found");
        } else {
            ImGui::OpenPopup("Not Found");
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Remove Book")) {
        if (library.removeBook(searchId)) {
            ImGui::OpenPopup("Removed");
        } else {
            ImGui::OpenPopup("Remove Failed");
        }
    }
    
    // Popups
    if (ImGui::BeginPopupModal("Book Found", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        Book* found = library.findBook(searchId);
        if (found) {
            ImGui::Text("ID: %d", found->id);
            ImGui::Text("Title: %s", found->title.c_str());
            ImGui::Text("Author: %s", found->author.c_str());
            ImGui::Text("Quantity: %d", found->quantity);
        }
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    if (ImGui::BeginPopupModal("Not Found", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Book with ID %d not found!", searchId);
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    if (ImGui::BeginPopupModal("Removed", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Book removed successfully!");
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    if (ImGui::BeginPopupModal("Remove Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Failed to remove book with ID %d", searchId);
        if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Display all books
    ImGui::SeparatorText("Library Inventory");
    
    if (ImGui::BeginTable("Books", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Author");
        ImGui::TableSetupColumn("Quantity");
        ImGui::TableHeadersRow();
        
        for (const auto& book : library.getBooks()) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%d", book.id);
            ImGui::TableNextColumn();
            ImGui::Text("%s", book.title.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", book.author.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%d", book.quantity);
        }
        
        ImGui::EndTable();
    }
    
    ImGui::Spacing();
    if (ImGui::Button("Exit Application"))
        glfwSetWindowShouldClose(window, true);
    
    ImGui::End();
}

void App::render()
{
    ImGui::Render();
    
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    glfwSwapBuffers(window);
}

void App::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (window)
    {
        glfwDestroyWindow(window);
    }
    glfwTerminate();
}