// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// Include our data structure visualizer components
#include "core/DataStructure.h"
#include "core/OperationManager.h"
#include "data_structure/ArrayStructure.h"
#include "data_structure/StackStructure.h"
#include "operation/ArrayOps.h"
#include "operation/StackOps.h"
#include "visual/GuiVisualizer.h"
#include <memory>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only
    GLFWwindow* window = glfwCreateWindow((int)(1280 * main_scale), (int)(800 * main_scale), "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 3
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.
#endif

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details. If you like the default font but want it to scale better, consider using the 'ProggyVector' from the same author!
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = false;
    bool show_visualizer_window = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Data structure visualizer state
    ArrayStructure arrayDS;
    StackStructure stackDS;
    OperationManager opManager;
    GuiVisualizer visualizer;

    // Demo controls
    static int insertIndex = 0;
    static int insertValue = 99;
    static int writeIndex = 0;
    static int writeValue = 42;
    static int moveFrom = 0;
    static int moveTo = 1;
    static int pushValue = 10;

    // Initialize array with some values
    std::vector<int> initialValues = {5, 2, 8, 1, 9};
    auto initOp = std::make_unique<ArrayInit>(initialValues);
    opManager.executeOperation(arrayDS, std::move(initOp));

    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Data Structure Visualizer Window
        if (show_visualizer_window)
        {
            ImGui::SetNextWindowSize(ImVec2(1000 * main_scale, 700 * main_scale), ImGuiCond_FirstUseEver);
            ImGui::Begin("Data Structure Visualizer", &show_visualizer_window);

            ImGui::Text("Interactive Data Structure Operations Demo");
            ImGui::Separator();

            // Tabs for different data structures
            if (ImGui::BeginTabBar("DataStructureTabs"))
            {
                // === ARRAY TAB ===
                if (ImGui::BeginTabItem("Array"))
                {
                    ImGui::Text("Array Operations:");
                    ImGui::Spacing();

                    // Display current array state
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Array:");
                    std::string arrayStr = "[ ";
                    for (size_t i = 0; i < arrayDS.data.size(); ++i) {
                        arrayStr += std::to_string(arrayDS.data[i]);
                        if (i < arrayDS.data.size() - 1) arrayStr += ", ";
                    }
                    arrayStr += " ]";
                    ImGui::Text("%s", arrayStr.c_str());
                    ImGui::Text("Size: %zu", arrayDS.data.size());
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    // Write Operation
                    ImGui::Text("Write Operation:");
                    ImGui::InputInt("Index##write", &writeIndex);
                    ImGui::InputInt("Value##write", &writeValue);
                    if (ImGui::Button("Write to Array")) {
                        if (writeIndex >= 0) {
                            auto writeOp = std::make_unique<WriteOp>(writeIndex, writeValue);
                            auto userOp = std::make_unique<UserOperation>("Manual Write", "Write value to index");
                            userOp->operations.push_back(std::move(writeOp));
                            opManager.executeOperation(arrayDS, std::move(userOp));
                        }
                    }
                    ImGui::Spacing();

                    // Move/Swap Operation
                    ImGui::Text("Move/Swap Operation:");
                    ImGui::InputInt("From Index##move", &moveFrom);
                    ImGui::InputInt("To Index##move", &moveTo);
                    if (ImGui::Button("Swap Elements")) {
                        if (moveFrom >= 0 && moveTo >= 0 &&
                            moveFrom < (int)arrayDS.data.size() && moveTo < (int)arrayDS.data.size()) {
                            auto moveOp = std::make_unique<MoveOp>(moveFrom, moveTo);
                            auto userOp = std::make_unique<UserOperation>("Manual Swap", "Swap two elements");
                            userOp->operations.push_back(std::move(moveOp));
                            opManager.executeOperation(arrayDS, std::move(userOp));
                        }
                    }
                    ImGui::Spacing();

                    // Insert Operation
                    ImGui::Text("Insert Operation:");
                    ImGui::InputInt("Index##insert", &insertIndex);
                    ImGui::InputInt("Value##insert", &insertValue);
                    if (ImGui::Button("Insert Element")) {
                        if (insertIndex >= 0 && insertIndex <= (int)arrayDS.data.size()) {
                            auto insertOp = std::make_unique<ArrayInsert>(arrayDS, insertIndex, insertValue);
                            opManager.executeOperation(arrayDS, std::move(insertOp));
                        }
                    }
                    ImGui::Spacing();

                    // Delete Operation
                    if (ImGui::Button("Delete First Element")) {
                        if (!arrayDS.data.empty()) {
                            auto deleteOp = std::make_unique<ArrayDelete>(arrayDS, 0);
                            opManager.executeOperation(arrayDS, std::move(deleteOp));
                        }
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Delete Last Element")) {
                        if (!arrayDS.data.empty()) {
                            auto deleteOp = std::make_unique<ArrayDelete>(arrayDS, arrayDS.data.size() - 1);
                            opManager.executeOperation(arrayDS, std::move(deleteOp));
                        }
                    }
                    ImGui::Spacing();

                    // Sort Operation
                    if (ImGui::Button("Sort Array (Bubble Sort)")) {
                        auto sortOp = std::make_unique<ArraySort>(arrayDS);
                        opManager.executeOperation(arrayDS, std::move(sortOp));
                    }
                    ImGui::Spacing();

                    // Reset Operation
                    if (ImGui::Button("Reset Array")) {
                        std::vector<int> resetValues = {5, 2, 8, 1, 9};
                        auto resetOp = std::make_unique<ArrayInit>(resetValues);
                        opManager.executeOperation(arrayDS, std::move(resetOp));
                    }

                    ImGui::EndTabItem();
                }

                // === STACK TAB ===
                if (ImGui::BeginTabItem("Stack"))
                {
                    ImGui::Text("Stack Operations:");
                    ImGui::Spacing();

                    // Display current stack state
                    ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Current Stack:");
                    std::stack<int> tempStack = stackDS.data;
                    std::vector<int> stackElements;
                    while (!tempStack.empty()) {
                        stackElements.push_back(tempStack.top());
                        tempStack.pop();
                    }

                    if (stackElements.empty()) {
                        ImGui::Text("(empty)");
                    } else {
                        ImGui::Text("Top -> [ ");
                        for (int val : stackElements) {
                            ImGui::SameLine();
                            ImGui::Text("%d ", val);
                        }
                        ImGui::SameLine();
                        ImGui::Text("]");
                    }
                    ImGui::Text("Size: %zu", stackDS.data.size());
                    ImGui::Spacing();
                    ImGui::Separator();
                    ImGui::Spacing();

                    // Push Operation
                    ImGui::Text("Push Operation:");
                    ImGui::InputInt("Value##push", &pushValue);
                    if (ImGui::Button("Push to Stack")) {
                        auto pushOp = std::make_unique<StackPush>(pushValue);
                        opManager.executeOperation(stackDS, std::move(pushOp));
                    }
                    ImGui::Spacing();

                    // Pop Operation
                    if (ImGui::Button("Pop from Stack")) {
                        if (!stackDS.data.empty()) {
                            auto popOp = std::make_unique<StackPop>();
                            opManager.executeOperation(stackDS, std::move(popOp));
                        }
                    }
                    ImGui::Spacing();

                    // Clear Operation
                    if (ImGui::Button("Clear Stack")) {
                        auto clearOp = std::make_unique<StackClear>(stackDS);
                        opManager.executeOperation(stackDS, std::move(clearOp));
                    }
                    ImGui::Spacing();

                    // Initialize Stack
                    if (ImGui::Button("Initialize Stack [1,2,3,4,5]")) {
                        std::vector<int> values = {1, 2, 3, 4, 5};
                        auto initOp = std::make_unique<StackInit>(values);
                        opManager.executeOperation(stackDS, std::move(initOp));
                    }

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::Separator();
            ImGui::Spacing();

            // Undo/Redo controls
            ImGui::Text("History Controls:");
            if (ImGui::Button("Undo")) {
                opManager.undo();
            }
            ImGui::SameLine();
            if (ImGui::Button("Redo")) {
                opManager.redo();
            }
            ImGui::SameLine();
            ImGui::Text("| Operations in history: %zu", opManager.getExecutedOperations().size());

            ImGui::Spacing();
            ImGui::Separator();

            // Display operation history
            ImGui::Text("Operation History:");
            ImGui::BeginChild("HistoryList", ImVec2(0, 150 * main_scale), true);
            const auto& history = opManager.getExecutedOperations();
            for (size_t i = 0; i < history.size(); ++i) {
                ImGui::Text("%zu: %s - %s", i + 1,
                           history[i]->getName().c_str(),
                           history[i]->getDescription().c_str());
            }
            ImGui::EndChild();

            ImGui::Spacing();
            ImGui::Checkbox("Show ImGui Demo", &show_demo_window);

            if (ImGui::Button("Quit Application"))
                glfwSetWindowShouldClose(window, GLFW_TRUE);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                       1000.0f / io.Framerate, io.Framerate);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
