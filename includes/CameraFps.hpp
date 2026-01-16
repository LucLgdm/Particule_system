/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CameraFps.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 16:31:34 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/09 17:24:52 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

struct GLFWwindow;

#include "global.hpp"

class CameraFps {
	public:
		CameraFps();
		~CameraFps();
		CameraFps(const CameraFps &other);
		
		CameraFps &operator=(const CameraFps &other);

		void update(GLFWwindow*);

		// Mouse interaction
		void processScroll(float);
		void beginRotate();
		void endRotate();
		void processMouseMove(float, float);
		
		glm::mat4 getViewMatrix() const { return _viewMatrix; };
		glm::mat4 getProjectionMatrix() const { return _projectionMatrix; };

		void updateProjectionMatrix(int width, int height) {
			_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
			_projectionMatrix = glm::perspective(
				glm::radians(_fov),
				_aspectRatio,
				0.1f,
				3000.0f
			);
		}

	private:
		glm::vec3 _position;
		glm::vec3 _target;
		glm::vec3 _up;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;
		
		float _fov;
		float _aspectRatio;
		
		bool  _rotating = false;
		bool  _firstMouse = true;
		float _lastX = 0.0f;
		float _lastY = 0.0f;

		float _yaw   = -90.0f;
		float _pitch = 0.0f;
		
};
