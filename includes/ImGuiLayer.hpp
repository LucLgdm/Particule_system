/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 14:18:59 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/16 14:26:35 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "ParticleSystem.hpp"

enum class CameraMode {
	ORBIT,
	FPS
};

#include "CameraOrbit.hpp"
#include "CameraFps.hpp"

class ImGuiLayer {
	public:
		ImGuiLayer();
		~ImGuiLayer();
		ImGuiLayer(const ImGuiLayer &other);
		
		ImGuiLayer &operator=(const ImGuiLayer &other);

		void initImGui(GLFWwindow*);
		void beginFrame();
		void render(ParticleSystem&, CameraMode&, CameraOrbit&);
		void renderPS(ParticleSystem&);
		void renderCamera(CameraMode&, CameraOrbit&);
		void endFrame();
		void shutdown();

		void renderAxisGizmo();
	private:

};
