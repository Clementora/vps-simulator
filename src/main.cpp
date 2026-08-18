#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <iostream>
#include "vps/physics/HeatSolver.hpp"

// Maps 25°C to Blue, and 230°C to Red
ImU32 GetHeatmapColor(double temp) {
    float norm = static_cast<float>((temp - 25.0) / (230.0 - 25.0));
    norm = std::clamp(norm, 0.0f, 1.0f);
    int r = static_cast<int>(255 * norm);
    int b = static_cast<int>(255 * (1.0f - norm));
    return IM_COL32(r, 0, b, 255);
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "VPS Simulator", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Simulation State
    HeatSolver solver(10, PCBProps()); // 10 nodes for the PCB thickness
    bool is_running = false;
    float elevator_y = 0.0f; // 0.0 is top, 1.0 is bottom

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Physics & Kinematics Step
        if (is_running) {
            if (elevator_y < 1.0f) elevator_y += 0.002f; // Lower elevator
            
            // If elevator is in the bottom half, it's inside the vapor
            double h_coeff = (elevator_y > 0.5f) ? 600.0 : 5.0; 
            solver.step(0.016, 230.0, h_coeff); // 16ms step size
        }

        // 2. Control Panel UI
        ImGui::Begin("Machine Controls");
        if (ImGui::Button(is_running ? "Pause" : "Start Cycle")) {
            is_running = !is_running;
        }
        if (ImGui::Button("Reset")) {
            is_running = false;
            elevator_y = 0.0f;
            solver = HeatSolver(10, PCBProps());
        }
        ImGui::Text("Surface Temp: %.1f C", solver.getNodeTemp(0));
        ImGui::Text("Core Temp: %.1f C", solver.getNodeTemp(solver.getNodeCount() / 2));
        ImGui::End();

        // 3. 2D Graphical Machine View

        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(250, 50), ImGuiCond_FirstUseEver);

        ImGui::Begin("Vapor Phase Chamber");
        ImVec2 p0 = ImGui::GetCursorScreenPos(); // Top-left of drawing area
        ImVec2 sz = ImGui::GetContentRegionAvail(); // Available size
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        // Draw machine background (dark grey)
        draw_list->AddRectFilled(p0, ImVec2(p0.x + sz.x, p0.y + sz.y), IM_COL32(40, 40, 40, 255));

        // Draw vapor zone (bottom half, translucent blue)
        float vapor_top = p0.y + (sz.y * 0.5f);
        draw_list->AddRectFilled(ImVec2(p0.x, vapor_top), ImVec2(p0.x + sz.x, p0.y + sz.y), IM_COL32(100, 200, 255, 60));

        // Calculate Elevator/PCB position
        float pcb_w = sz.x * 0.6f;
        float pcb_h = 30.0f;
        float pcb_x = p0.x + (sz.x - pcb_w) * 0.5f; // Center horizontally
        float pcb_y = p0.y + 20.0f + (sz.y - 80.0f) * elevator_y;

        // Draw the PCB nodes as a gradient grid
        int nodes = solver.getNodeCount();
        float node_w = pcb_w / nodes;
        for (int i = 0; i < nodes; ++i) {
            ImU32 color = GetHeatmapColor(solver.getNodeTemp(i));
            draw_list->AddRectFilled(
                ImVec2(pcb_x + i * node_w, pcb_y),
                ImVec2(pcb_x + (i + 1) * node_w, pcb_y + pcb_h),
                color
            );
        }
        ImGui::End();

        // Render Frame
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}