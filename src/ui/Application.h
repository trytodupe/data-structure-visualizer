#pragma once

#include "../core/DataStructure.h"
#include "../core/DynamicArray.h"
#include "../visualization/LinearRenderer.h"
#include "imgui.h"
#include <memory>
#include <string>

class Application {
private:
    std::unique_ptr<DataStructure> currentDataStructure;
    std::unique_ptr<LinearRenderer> linearRenderer;
    
    // UI state
    std::string selectedStructureType = "DynamicArray";
    int inputValue = 0;
    int inputIndex = 0;
    char inputBuffer[256] = "";
    
    // Window flags
    bool showDemo = false;
    
public:
    Application() {
        // Initialize with default array
        currentDataStructure = std::make_unique<DynamicArray>();
        linearRenderer = std::make_unique<LinearRenderer>();
        
        // Initialize with some sample data
        std::vector<int> sampleData = {10, 20, 30, 40, 50};
        currentDataStructure->initialize(sampleData);
    }
    
    ~Application() = default;
    
    void render() {
        // Create main dockspace
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
                                      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
                                      ImGuiWindowFlags_NoBackground;
        
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        
        ImGui::Begin("DockSpace", nullptr, windowFlags);
        ImGui::PopStyleVar(3);
        
        // Create dockspace
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        
        ImGui::End();
        
        // Render control panel
        renderControlPanel();
        
        // Render visualization panel
        renderVisualizationPanel();
        
        // Render operation history panel
        renderOperationHistoryPanel();
        
        // Show demo if requested
        if (showDemo) {
            ImGui::ShowDemoWindow(&showDemo);
        }
    }
    
private:
    void renderControlPanel() {
        if (ImGui::Begin("Control Panel")) {
            // Data structure selection
            ImGui::Text("Data Structure Type:");
            const char* structureTypes[] = {"DynamicArray"};
            int currentType = 0;
            if (ImGui::Combo("##StructureType", &currentType, structureTypes, IM_ARRAYSIZE(structureTypes))) {
                // For Phase 1, only DynamicArray is supported
                selectedStructureType = structureTypes[currentType];
            }
            
            ImGui::Separator();
            
            // Initialize section
            ImGui::Text("Initialize Array:");
            ImGui::InputText("##InitData", inputBuffer, sizeof(inputBuffer));
            ImGui::SameLine();
            if (ImGui::Button("Initialize")) {
                std::vector<int> initData = parseCommaSeparatedInts(inputBuffer);
                currentDataStructure->initialize(initData);
            }
            
            ImGui::Separator();
            
            // Operations section
            ImGui::Text("Operations:");
            
            // Insert operation
            ImGui::InputInt("Value##Insert", &inputValue);
            ImGui::InputInt("Index##Insert", &inputIndex);
            if (ImGui::Button("Insert")) {
                if (auto array = dynamic_cast<DynamicArray*>(currentDataStructure.get())) {
                    array->insert(inputIndex, inputValue);
                }
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Push Back")) {
                if (auto array = dynamic_cast<DynamicArray*>(currentDataStructure.get())) {
                    array->pushBack(inputValue);
                }
            }
            
            // Delete operation
            if (ImGui::Button("Delete at Index")) {
                if (auto array = dynamic_cast<DynamicArray*>(currentDataStructure.get())) {
                    array->deleteAt(inputIndex);
                }
            }
            
            ImGui::SameLine();
            if (ImGui::Button("Clear All")) {
                currentDataStructure->clear();
            }
            
            ImGui::Separator();
            
            // Debug options
            if (ImGui::Button("Show ImGui Demo")) {
                showDemo = !showDemo;
            }
            
            // Info section
            ImGui::Separator();
            ImGui::Text("Structure Info:");
            ImGui::Text("Type: %s", currentDataStructure->getType().c_str());
            ImGui::Text("Size: %zu", currentDataStructure->size());
            ImGui::Text("Empty: %s", currentDataStructure->empty() ? "Yes" : "No");
        }
        ImGui::End();
    }
    
    void renderVisualizationPanel() {
        if (ImGui::Begin("Visualization")) {
            ImVec2 contentRegion = ImGui::GetContentRegionAvail();
            ImVec2 position = ImGui::GetCursorScreenPos();
            
            // Render the current data structure
            linearRenderer->render(currentDataStructure.get(), position, contentRegion);
            
            // Reserve space for the visualization
            ImVec2 requiredSize = linearRenderer->getRequiredSize(currentDataStructure.get());
            ImGui::Dummy(requiredSize);
        }
        ImGui::End();
    }
    
    void renderOperationHistoryPanel() {
        if (ImGui::Begin("Operation History")) {
            auto operations = currentDataStructure->getOperationHistory();
            
            if (operations.empty()) {
                ImGui::Text("No operations performed yet.");
            } else {
                // Show operations in reverse order (most recent first)
                for (auto it = operations.rbegin(); it != operations.rend(); ++it) {
                    const auto& op = *it;
                    
                    // Color code based on success
                    ImVec4 color = op.success ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
                    
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::Text("%s", op.description.c_str());
                    ImGui::PopStyleColor();
                }
            }
            
            ImGui::Separator();
            if (ImGui::Button("Clear History")) {
                currentDataStructure->clearOperationHistory();
            }
        }
        ImGui::End();
    }
    
    std::vector<int> parseCommaSeparatedInts(const std::string& input) {
        std::vector<int> result;
        std::string current = "";
        
        for (char c : input) {
            if (c == ',' || c == ' ') {
                if (!current.empty()) {
                    try {
                        result.push_back(std::stoi(current));
                    } catch (const std::exception&) {
                        // Skip invalid numbers
                    }
                    current = "";
                }
            } else if (std::isdigit(c) || c == '-') {
                current += c;
            }
        }
        
        // Handle last number
        if (!current.empty()) {
            try {
                result.push_back(std::stoi(current));
            } catch (const std::exception&) {
                // Skip invalid number
            }
        }
        
        return result;
    }
};