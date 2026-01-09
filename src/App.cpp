#include "App.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <string>

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
    glfwSwapInterval(1); // Enable V-Sync
}

void App::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Apply a base dark style, then we will tweak it in applyTheme()
    ImGui::StyleColorsDark();

    const char* glsl_version = "#version 330";
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    applyTheme();
}

void App::applyTheme()
{
    // Make the UI look softer and more modern
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f; // Fullscreen windows shouldn't have corners
    style.ChildRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.ScrollbarRounding = 5.0f;
    style.GrabRounding = 5.0f;
    
    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(10, 8);
    style.ItemSpacing = ImVec2(10, 10);
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
    static std::string statusMessage = ""; 

    // --- SETUP FULL SCREEN WINDOW ---
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | 
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | 
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::Begin("Background", nullptr, window_flags);

    // --- TITLE BAR ---
    ImGui::TextDisabled("LIBRARY MANAGEMENT SYSTEM v1.0");
    ImGui::Separator();
    ImGui::Spacing();

    // --- LEFT PANEL (CONTROLS) ---
    float leftPanelWidth = ImGui::GetContentRegionAvail().x * 0.3f;
    
    ImGui::BeginChild("LeftPanel", ImVec2(leftPanelWidth, 0), true);
    
    // 1. ADD BOOK SECTION
    ImGui::SeparatorText("New Entry");
    ImGui::Spacing();
    
    ImGui::Text("Book Details:");
    ImGui::InputInt("ID##New", &bookId);
    ImGui::InputTextWithHint("Title", "Enter book title", titleBuf, IM_ARRAYSIZE(titleBuf));
    ImGui::InputTextWithHint("Author", "Enter author name", authorBuf, IM_ARRAYSIZE(authorBuf));
    ImGui::InputInt("Qty", &quantity);
    
    ImGui::Spacing();
    
    // Green Button for Add
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.33f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.33f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.33f, 0.8f, 0.8f));
    if (ImGui::Button("Add Book to Library", ImVec2(-FLT_MIN, 40))) { 
        if (strlen(titleBuf) > 0 && strlen(authorBuf) > 0) {
            library.addBook({bookId, std::string(titleBuf), std::string(authorBuf), quantity});
            statusMessage = "Added: " + std::string(titleBuf);
            bookId++;
            titleBuf[0] = '\0';
            authorBuf[0] = '\0';
            quantity = 1;
        } else {
            ImGui::OpenPopup("Error");
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // 2. MANAGE SECTION
    ImGui::SeparatorText("Management");
    ImGui::Spacing();
    
    ImGui::InputInt("Target ID", &searchId);
    ImGui::Spacing();
    
    float btnWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;
    
    // Blue Button for Search
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.6f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.6f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.6f, 0.8f, 0.8f));
    if (ImGui::Button("Find Book", ImVec2(btnWidth, 35))) {
        Book* found = library.findBook(searchId);
        if (found) {
            ImGui::OpenPopup("Book Found");
        } else {
            ImGui::OpenPopup("Not Found");
        }
    }
    ImGui::PopStyleColor(3);

    ImGui::SameLine();
    
    // Red Button for Remove
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
    if (ImGui::Button("Remove Book", ImVec2(btnWidth, 35))) {
        if (library.removeBook(searchId)) {
            statusMessage = "Removed Book ID: " + std::to_string(searchId);
            ImGui::OpenPopup("Removed");
        } else {
            ImGui::OpenPopup("Not Found");
        }
    }
    ImGui::PopStyleColor(3);

    // Status Message
    if (!statusMessage.empty()) {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Status: %s", statusMessage.c_str());
    }
    
    // Exit Button
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 50);
    if (ImGui::Button("Exit Application", ImVec2(-FLT_MIN, 30))) {
        glfwSetWindowShouldClose(window, true);
    }

    // --- POPUPS (NOW INSIDE THE LEFT PANEL SCOPE) ---
    // These must be here to "see" the OpenPopup calls made by the buttons above
    
    if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Title and Author cannot be empty!");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Book Found", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        Book* found = library.findBook(searchId);
        if (found) {
            ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Book Located in Database:");
            ImGui::Separator();
            ImGui::Text("ID:       %d", found->id);
            ImGui::Text("Title:    %s", found->title.c_str());
            ImGui::Text("Author:   %s", found->author.c_str());
            ImGui::Text("Quantity: %d", found->quantity);
        }
        ImGui::Separator();
        if (ImGui::Button("Close", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
    
    if (ImGui::BeginPopupModal("Not Found", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Operation Failed");
        ImGui::Text("Book with ID %d does not exist.", searchId);
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Removed", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Success");
        ImGui::Text("Book removed successfully.");
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    ImGui::EndChild(); // End Left Panel

    ImGui::SameLine(); // Move to the right

    // --- RIGHT PANEL (TABLE) ---
    ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
    
    ImGui::Text("Current Inventory");
    
    ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | 
                                 ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY | 
                                 ImGuiTableFlags_SizingStretchProp;
                                 
    if (ImGui::BeginTable("InventoryTable", 4, tableFlags)) {
        ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 50.0f);
        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Author");
        ImGui::TableSetupColumn("Qty", ImGuiTableColumnFlags_WidthFixed, 50.0f);
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
    
    ImGui::EndChild(); // End Right Panel

    ImGui::End(); // End Main Window
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