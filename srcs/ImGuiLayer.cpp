/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:18:57 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/09 14:34:43 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ImGuiLayer.hpp"

// Constructeur
ImGuiLayer::ImGuiLayer() {}

ImGuiLayer::~ImGuiLayer() {}

ImGuiLayer::ImGuiLayer(const ImGuiLayer &other) {
    *this = other;
}

ImGuiLayer &ImGuiLayer::operator=(const ImGuiLayer &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}

// Initialize ImGui context and setup for GLFW and OpenGL
void ImGuiLayer::initImGui(GLFWwindow* window) {
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 330");
}

// Start a new ImGui frame
void ImGuiLayer::beginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

// Render ImGui draw data
void ImGuiLayer::render(ParticleSystem& system) {
	ImGui::Begin("Particle System Controls");

	// static float radius = 1.0f;
	// if (ImGui::SliderFloat("Radius", &radius, 0.1f, 5.0f)) {
	// 	// Update radius in the particle system if needed
	// }

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
		1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();
}

// End the ImGui frame
void ImGuiLayer::endFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Cleanup ImGui resources
void ImGuiLayer::shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

