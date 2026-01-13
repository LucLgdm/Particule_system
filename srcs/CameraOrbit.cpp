/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CameraOrbit.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 14:10:42 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/12 14:10:43 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CameraOrbit.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

CameraOrbit::CameraOrbit(): _target(0.0f, 0.0f, 0.0f), _distance(7.0f), _yaw(-90.0f),
			_pitch(0.0f), _moveSpeed(0.02f), _up(0.0f, 1.0f, 0.0f), _fov(45.0f), _rotating(false),
			_firstMouse(true) {
	_aspectRatio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
	_projectionMatrix = glm::perspective(
		glm::radians(_fov),
		_aspectRatio,
		0.1f,
		100.0f
	);

	updateView();
}

CameraOrbit::~CameraOrbit() {}

void CameraOrbit::update(GLFWwindow* window) {
	glm::vec3 forward = normalize(_target - _position);
	glm::vec3 right = normalize(cross(forward, _up));


	// Update key states
	for(auto &keyPair : _keys) {
		keyPair.second.update(glfwGetKey(window, keyPair.first) == GLFW_PRESS);
	}

	if (_keys[GLFW_KEY_W].isDown) _target +=  forward * _moveSpeed;
	if (_keys[GLFW_KEY_S].isDown) _target -=  forward * _moveSpeed;
	if (_keys[GLFW_KEY_A].isDown) _target -=  right * _moveSpeed;
	if (_keys[GLFW_KEY_D].isDown) _target +=  right * _moveSpeed;
	
	updateView();
}

void CameraOrbit::updateView() {
	glm::vec3 offset;
	offset.x = _distance * cos(glm::radians(_pitch)) * cos(glm::radians(_yaw));
	offset.y = _distance * sin(glm::radians(_pitch));
	offset.z = _distance * cos(glm::radians(_pitch)) * sin(glm::radians(_yaw));

	_position = _target + offset;
	_viewMatrix = glm::lookAt(_position, _target, _up);
}

void CameraOrbit::processScroll(float yoffset) {
	_distance -= yoffset * 0.5f;
	_distance = glm::clamp(_distance, 1.0f, 50.0f);
	updateView();
}

void CameraOrbit::beginRotate() {
	_rotating = true;
	_firstMouse = true;
}

void CameraOrbit::endRotate() {
	_rotating = false;
}

void CameraOrbit::processMouseMove(float xpos, float ypos) {
	if (!_rotating)
		return;

	if (_firstMouse) {
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
		return;
	}

	float dx = xpos - _lastX;
	float dy = _lastY - ypos;

	_lastX = xpos;
	_lastY = ypos;

	float sensitivity = 0.2f;
	_yaw   += dx * sensitivity;
	_pitch += dy * sensitivity;

	_pitch = glm::clamp(_pitch, -89.0f, 89.0f);

	updateView();
}
