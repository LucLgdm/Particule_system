/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:18:57 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/10 14:20:20 by lde-merc         ###   ########.fr       */
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
	
	renderAxisGizmo(cameraOrbit);
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
		system.setGravity(gravityEnable);
	}


	// static int uiPartCount = 0;
	// static bool nChange = false;
	// if (ImGui::SliderInt("Particle count", &uiPartCount, 1, 5000000))
	// 	nChange = true;
		
	// static bool resetShpere = false, resetCube = false, resetPyramid = false;
	// if (ImGui::Checkbox("Sphere", &resetShpere)) resetCube = false, resetPyramid = false;
	// ImGui::SameLine();
	// if (ImGui::Checkbox("Cube", &resetCube)) resetShpere = false, resetPyramid = false;
	// ImGui::SameLine();
	// if (ImGui::Checkbox("Pyramid", &resetPyramid)) resetShpere = false, resetCube = false;
	
	// static bool initSpeed_1 = false, initSpeed_2 = false;
	// if (ImGui::Checkbox("Speed 1", &initSpeed_1)) initSpeed_2 = false;
	// ImGui::SameLine();
	// if (ImGui::Checkbox("Speed 2", &initSpeed_2)) initSpeed_1 = false;


	// ImGui::Button("Reset Particles");
	// if (ImGui::IsItemClicked() && (resetShpere || resetCube || resetPyramid) && (initSpeed_1 || initSpeed_2)) {
	// 	system.setSpeed(initSpeed_1 ? 1 : 2);
	// 	if (nChange) {
	// 		system.setNbPart(uiPartCount);
	// 		nChange = false;
	// 	}
	// 	if (resetShpere) {
	// 		system.initializeShape("sphere");
	// 		resetShpere = false;
	// 	}else if (resetCube) {
	// 		system.initializeShape("cube");
	// 		resetCube = false;
	// 	}else {
	// 		system.initializeShape("pyramid");
	// 		resetPyramid = false;
	// 	}
	// 	initSpeed_1 = initSpeed_2 = false;
	// }

	static int  uiPartCount = system.getNPart();
	static int  uiShape = 0;   // 0 sphere, 1 cube, 2 pyramid
	static int  uiSpeed = 1;

	ImGui::SliderInt("Particle count", &uiPartCount, 1, 5'000'000);

	ImGui::RadioButton("Sphere",   &uiShape, 0); ImGui::SameLine();
	ImGui::RadioButton("Cube",     &uiShape, 1); ImGui::SameLine();
	ImGui::RadioButton("Pyramid",  &uiShape, 2);

	ImGui::RadioButton("Speed 1", &uiSpeed, 1); ImGui::SameLine();
	ImGui::RadioButton("Speed 2", &uiSpeed, 2);


	if (ImGui::Button("Reset Particles")) {
		system.setSpeed(uiSpeed);
		system.setNbPart(uiPartCount);

		switch (uiShape) {
				case 0: system.initializeShape("sphere");  break;
				case 1: system.initializeShape("cube");    break;
				case 2: system.initializeShape("pyramid"); break;
			}
	}

	
	static bool color1 = true, color2 = false;
	if (ImGui::Checkbox("Color 1", &color1)) color2 = false;
	ImGui::SameLine();
	if (ImGui::Checkbox("Color 2", &color2)) color1 = false;
	if (color1) {
		system.setColorMode(0);
	} else if (color2) {
		system.setColorMode(1);
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

	glm::vec3 pos = cameraOrbit.getPosition();
	ImGui::Text("Cam X: %.2f, Cam Y: %.2f, Cam Z: %.2f", pos.x, pos.y, pos.z);
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

void ImGuiLayer::renderAxisGizmo(const CameraOrbit& camera) {
	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 150, ImGui::GetIO().DisplaySize.y - 150), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(140, 140), ImGuiCond_Always);
	ImGui::Begin("##AxisGizmo", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
	
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 canvas_pos = ImGui::GetCursorScreenPos();
	ImVec2 canvas_size = ImGui::GetContentRegionAvail();
	
	ImVec2 center = ImVec2(canvas_pos.x + canvas_size.x * 0.5f, canvas_pos.y + canvas_size.y * 0.5f);
	float axisLength = 40.0f;
	
	// Les axes du monde en coordonnées homogènes
	glm::vec4 xAxis = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec4 yAxis = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	glm::vec4 zAxis = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	
	// Note: à adapter si vous avez accès à la caméra via un paramètre
	// Pour maintenant, vous devrez passer la matrice de vue en paramètre
	
	// Appliquer la rotation de la caméra aux axes
	glm::mat4 viewMatrix = camera.getViewMatrix();
	glm::mat3 rotation = glm::mat3(viewMatrix);
	xAxis = glm::vec4(rotation * glm::vec3(xAxis), 0.0f);
	yAxis = glm::vec4(rotation * glm::vec3(yAxis), 0.0f);
	zAxis = glm::vec4(rotation * glm::vec3(zAxis), 0.0f);
	
	// Conversion en coordonnées écran (projection 2D simple)
	auto projectAxis = [&center, &axisLength](glm::vec3 axis) -> ImVec2 {
		return ImVec2(center.x + axis.x * axisLength, center.y - axis.y * axisLength);
	};
	
	ImVec2 xEnd = projectAxis(glm::vec3(xAxis));
	ImVec2 yEnd = projectAxis(glm::vec3(yAxis));
	ImVec2 zEnd = projectAxis(glm::vec3(zAxis));
	
	// X axis (Red)
	draw_list->AddLine(center, xEnd, IM_COL32(255, 0, 0, 255), 2.0f);
	draw_list->AddText(ImVec2(xEnd.x + 5, xEnd.y - 5), IM_COL32(255, 0, 0, 255), "X");
	
	// Y axis (Green)
	draw_list->AddLine(center, yEnd, IM_COL32(0, 255, 0, 255), 2.0f);
	draw_list->AddText(ImVec2(yEnd.x - 10, yEnd.y - 10), IM_COL32(0, 255, 0, 255), "Y");
	
	// Z axis (Blue)
	draw_list->AddLine(center, zEnd, IM_COL32(0, 0, 255, 255), 2.0f);
	draw_list->AddText(ImVec2(zEnd.x + 5, zEnd.y + 5), IM_COL32(0, 0, 255, 255), "Z");
	
	ImGui::End();
}

