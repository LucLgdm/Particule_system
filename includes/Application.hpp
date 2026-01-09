/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:42:54 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/09 13:46:59 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

static int HEIGHT = 1200;
static int WIDTH = 1600;

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <memory>

#include "exception.hpp"
#include "ParticleSystem.hpp"
#include "ImGuiLayer.hpp"
#include "Camera.hpp"

class Application {
	public:
		Application();
		~Application();
		Application(const Application &other);
		
		Application &operator=(const Application &other);

		void init(int, char **);
		void initShader();
		void run();
		void cleanup();

		void handleFps();

	private:
		GLFWwindow* _window;
		std::unique_ptr<ParticleSystem> _system; // More modern and safer: avoids leaks
		ImGuiLayer	_imguiLayer;
		Camera		_camera;
		int 	_nbParticle;
		string 	_shape;
		float 	_lastFrameTime;
		float 	_lastFpsTime;
		int		_fps;

		GLuint _shaderProgram;

		void checkinput(int, char**);
		void initGLFW();
		void initOpenGL();
};
