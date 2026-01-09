/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:18:57 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/09 16:08:22 by lde-merc         ###   ########.fr       */
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


/* 
	ImGui::Begin / End
	ImGui::Button
	ImGui::Checkbox
	ImGui::SliderFloat
	ImGui::Text
*/
// Render ImGui draw data
void ImGuiLayer::render(ParticleSystem& system) {
	ImGui::Begin("Particle System Controls");

	static bool gravityEnable = 0;
	if (ImGui::Checkbox("Enable Gravity", &gravityEnable)) {
		// Update gravity enable in the particle system
		system.setGravity(gravityEnable);
	}

	static bool resetShpere = false, resetCube = false;
	ImGui::Checkbox("Sphere", &resetShpere);
	ImGui::SameLine();
	ImGui::Checkbox("Cube", &resetCube);
	
	ImGui::Button("Reset Particles");
	if (ImGui::IsItemClicked() && (resetShpere || resetCube)) {
		if (resetShpere) {
			system.initializeShape("sphere");
			resetShpere = false;
		}else {
			system.initializeShape("cube");
			resetCube = false;
		}
	}

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

