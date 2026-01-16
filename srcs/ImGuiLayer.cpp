/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:18:57 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/16 15:41:57 by lde-merc         ###   ########.fr       */
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
void ImGuiLayer::render(ParticleSystem& system, CameraMode& cameraMode, CameraOrbit& cameraOrbit) {
	ImGui::Begin("Particle System Controls");

	renderCamera(cameraMode, cameraOrbit);
	renderPS(system);

	ImGui::End();
	
	renderAxisGizmo();
}

static int editingIndex = -1;

void ImGuiLayer::renderPS(ParticleSystem& system) {
	auto& gPoint = system.getGravityPoint();
	ImGui::Text("Gravity Centers: %d / 8", system.getNGravityPos());

	for (int i = 0; i < gPoint.size(); ++i) {
		if (gPoint[i].getState())
			ImGui::TextColored(ImVec4(0,1,0,1),
				"Active  Pos: %.2f %.2f %.2f, mass: %.2f",
				gPoint[i].getx(), gPoint[i].gety(), gPoint[i].getz(), gPoint[i].getMass());
		else
			ImGui::TextColored(ImVec4(1,0,0,1),
				"Inactive Pos: %.2f %.2f %.2f, mass: %.2f",
				gPoint[i].getx(), gPoint[i].gety(), gPoint[i].getz(), gPoint[i].getMass());
		ImGui::SameLine();
		if (ImGui::Button(("Toggle##" + std::to_string(i)).c_str())) {
			gPoint[i].active = (gPoint[i].active == 0) ? 1 : 0;
			system.updateGravityBuffer();
		}
		ImGui::SameLine();
		if (ImGui::Button(("Remove##" + std::to_string(i)).c_str())) {
			// gPoint.erase(gPoint.begin() + i);
			system.removeGravityPoint(i);
			std::cout << "Removed gravity point at index " << i << std::endl;
			if (editingIndex == i) editingIndex = -1;
			continue;
		}
		ImGui::SameLine();
		if (ImGui::Button(("Modify##" + std::to_string(i)).c_str())) {
			editingIndex = (editingIndex == i) ? -1 : i;
		}


		if (editingIndex == i) {
			ImGui::Indent();
			
			float x = gPoint[i].getx();
			if (ImGui::DragFloat(("X##" + std::to_string(i)).c_str(), &x, 0.1f)) {
				float newPos[4] = {x, gPoint[i].gety(), gPoint[i].getz(), gPoint[i].getMass()};
				gPoint[i].setPos(newPos);
				system.updateGravityBuffer();
			}
			
			float y = gPoint[i].gety();
			if (ImGui::DragFloat(("Y##" + std::to_string(i)).c_str(), &y, 0.1f)) {
				float newPos[4] = {gPoint[i].getx(), y, gPoint[i].getz(), gPoint[i].getMass()};
				gPoint[i].setPos(newPos);
				system.updateGravityBuffer();
			}
			
			float z = gPoint[i].getz();
			if (ImGui::DragFloat(("Z##" + std::to_string(i)).c_str(), &z, 0.1f)) {
				float newPos[4] = {gPoint[i].getx(), gPoint[i].gety(), z, gPoint[i].getMass()};
				gPoint[i].setPos(newPos);
				system.updateGravityBuffer();
			}
			
			float mass = gPoint[i].getMass();
			if (ImGui::DragFloat(("Mass##" + std::to_string(i)).c_str(), &mass, 0.1f, 0.0f, 1000.0f)) {
				float newPos[4] = {gPoint[i].getx(), gPoint[i].gety(), gPoint[i].getz(), mass};
				gPoint[i].setPos(newPos);
				system.updateGravityBuffer();
			}
			
			ImGui::Unindent();
		}
	}

	static float newPos[4] = {0.f, 0.f, 0.f, 0.f};
	ImGui::InputFloat4("New gravity position and mass", newPos);

	if (ImGui::Button("Add gravity center")) {
		system.addGravityPoint(newPos[0], newPos[1], newPos[2], newPos[3]);
	}

	
	static bool gravityEnable = 0;
	if (ImGui::Checkbox("Enable Gravity", &gravityEnable)) {
		// Update gravity enable in the particle system
		system.setGravity(gravityEnable);
	}


	static bool resetShpere = false, resetCube = false, resetPyramid = false;
	ImGui::Checkbox("Sphere", &resetShpere);
	ImGui::SameLine();
	ImGui::Checkbox("Cube", &resetCube);
	ImGui::SameLine();
	ImGui::Checkbox("Pyramid", &resetPyramid);

	ImGui::Button("Reset Particles");
	if (ImGui::IsItemClicked() && (resetShpere || resetCube || resetPyramid)) {
		if (resetShpere) {
			system.initializeShape("sphere");
			resetShpere = false;
		}else if (resetCube) {
			system.initializeShape("cube");
			resetCube = false;
		}else {
			system.initializeShape("pyramid");
			resetPyramid = false;
		}
	}

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
		1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}

void ImGuiLayer::renderCamera(CameraMode& cameraMode, CameraOrbit& cameraOrbit) {
	const char* items[] = { "Orbit", "Fps" };
	static int current_item = (cameraMode == CameraMode::ORBIT) ? 0 : 1;

	if (ImGui::Combo("Camera Mode", &current_item, items, IM_ARRAYSIZE(items))) {
		if (current_item == 0)
			cameraMode = CameraMode::ORBIT;
		else
			cameraMode = CameraMode::FPS;
	}
	float speed = cameraOrbit.getSpeed();
	ImGui::SliderFloat("Camera Speed", &speed, 0.01f, 1.0f);
	if (speed != cameraOrbit.getSpeed()) {
		cameraOrbit.setSpeed(speed);
	}

	ImGui::Text("Mouse X: %.2f, Mouse Y: %.2f", cameraOrbit.getMouseX(), cameraOrbit.getMouseY());
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

void ImGuiLayer::renderAxisGizmo() {
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 150, ImGui::GetIO().DisplaySize.y - 150), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(140, 140), ImGuiCond_Always);
    ImGui::Begin("##AxisGizmo", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();
    
    ImVec2 center = ImVec2(canvas_pos.x + canvas_size.x * 0.5f, canvas_pos.y + canvas_size.y * 0.5f);
    float axisLength = 40.0f;
    
    // X axis (Red)
    draw_list->AddLine(center, ImVec2(center.x + axisLength, center.y), IM_COL32(255, 0, 0, 255), 2.0f);
    draw_list->AddText(ImVec2(center.x + axisLength + 5, center.y - 5), IM_COL32(255, 0, 0, 255), "X");
    
    // Y axis (Green)
    draw_list->AddLine(center, ImVec2(center.x, center.y - axisLength), IM_COL32(0, 255, 0, 255), 2.0f);
    draw_list->AddText(ImVec2(center.x - 10, center.y - axisLength - 10), IM_COL32(0, 255, 0, 255), "Y");
    
    // Z axis (Blue)
    draw_list->AddLine(center, ImVec2(center.x + axisLength * 0.7f, center.y + axisLength * 0.7f), IM_COL32(0, 0, 255, 255), 2.0f);
    draw_list->AddText(ImVec2(center.x + axisLength * 0.7f + 5, center.y + axisLength * 0.7f + 5), IM_COL32(0, 0, 255, 255), "Z");
    
    ImGui::End();
}

