/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CameraOrbit.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 14:10:42 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/12 14:10:43 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "global.hpp"

#include <map>

struct GLFWwindow;

class CameraOrbit {
	public:
		CameraOrbit();
		~CameraOrbit();

		void update(GLFWwindow*);
		void processMouseMove(float xpos, float ypos);
		void processScroll(float yoffset);
		void beginRotate();
		void endRotate();

		const glm::mat4& getViewMatrix() const { return _viewMatrix; }
		const glm::mat4& getProjectionMatrix() const { return _projectionMatrix; }
		const float getSpeed() const { return _moveSpeed; }
		void setSpeed(float speed) { _moveSpeed = speed; }

		float getMouseX() const { return _lastX; }
		float getMouseY() const { return _lastY; }

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
		// Orbite
		glm::vec3 _target;
		float _distance;
		float _yaw;
		float _pitch;
		float _moveSpeed;

		// Camera
		glm::vec3 _position;
		glm::vec3 _up;

		// Projection
		float _fov;
		float _aspectRatio;
		glm::mat4 _viewMatrix;
		glm::mat4 _projectionMatrix;

		// Mouse
		bool  _rotating;
		bool  _firstMouse;
		float _lastX;
		float _lastY;
		
		// Keys
		std::map<int, KeyState> _keys;

		void updateView();

};
