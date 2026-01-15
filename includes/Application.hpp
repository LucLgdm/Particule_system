/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:42:54 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/15 17:47:39 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include <memory>

#include "exception.hpp"
#include "ParticleSystem.hpp"
#include "ImGuiLayer.hpp"



class Application {
	public:
		Application();
		~Application();
		Application(const Application &other);
		
		Application &operator=(const Application &other);

		void init(int, char **);
		void setCallbacks();
		void initShader();
		void run();
		void cleanup();

		glm::mat4 getViewMatrix() const {
			if (_cameraMode == CameraMode::FPS)
				return _cameraFps.getViewMatrix();
			else
				return _cameraOrbit.getViewMatrix();
		}

		glm::mat4 getProjectionMatrix() const {
			if (_cameraMode == CameraMode::FPS)
				return _cameraFps.getProjectionMatrix();
			else
				return _cameraOrbit.getProjectionMatrix();
		}

		void updateCam() {
			if (_cameraMode == CameraMode::FPS)
				_cameraFps.update(_window);
			else
				_cameraOrbit.update(_window);
		}
		void handleFps();

		// Public members for callback access
		CameraMode		_cameraMode = CameraMode::ORBIT;
		CameraFps		_cameraFps;
		CameraOrbit		_cameraOrbit;

	private:
		GLFWwindow* _window;
		std::unique_ptr<ParticleSystem> _system; // More modern and safer: avoids leaks
		ImGuiLayer	_imguiLayer;
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
