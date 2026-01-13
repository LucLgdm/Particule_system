/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CameraFps.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/09 16:32:18 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/09 17:28:27 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CameraFps.hpp"

// Constructeur
CameraFps::CameraFps(): _position(0.0f, 0.0f, 3.0f), _target(0.0f, 0.0f, 0.0f), _up(0.0f, 1.0f, 0.0f), _fov(45.0f){
	_aspectRatio = static_cast<float>(WIDTH) / static_cast<float>(HEIGHT);
	_viewMatrix = glm::lookAt(_position, _target, _up);
	_projectionMatrix = glm::perspective(glm::radians(_fov), _aspectRatio, 0.1f, 100.0f);
}

CameraFps::~CameraFps() {}

CameraFps::CameraFps(const CameraFps &other) {
    *this = other;
}

CameraFps &CameraFps::operator=(const CameraFps &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}

void CameraFps::update(GLFWwindow* window) {
	// Here you can implement CameraFps controls (e.g., WASD for movement, mouse for looking around)
	// For simplicity, we'll keep the CameraFps static in this example

	_viewMatrix = glm::lookAt(_position, _target, _up);
	_projectionMatrix = glm::perspective(glm::radians(_fov), _aspectRatio, 0.1f, 100.0f);
}

void CameraFps::processScroll(float yoffset) {
    float zoomSpeed = 0.5f; // ajustable
    glm::vec3 direction = glm::normalize(_target - _position);

    // avancer/reculer la position le long du vecteur vers le target
    _position += direction * yoffset * zoomSpeed;

    // mettre à jour la matrice de vue
    _viewMatrix = glm::lookAt(_position, _target, _up);
}

void CameraFps::beginRotate() {
	_rotating = true;
	_firstMouse = true;
}

void CameraFps::endRotate() {
	_rotating = false;
}

void CameraFps::processMouseMove(float xpos, float ypos) {
	if (!_rotating)
		return;

	if (_firstMouse) {
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
		return;
	}

	float dx = xpos - _lastX;
	float dy = _lastY - ypos; // inversé

	_lastX = xpos;
	_lastY = ypos;

	float sensitivity = 0.2f;
	_yaw   += dx * sensitivity;
	_pitch += dy * sensitivity;

	_pitch = glm::clamp(_pitch, -89.0f, 89.0f);

	// recalcul de la direction
	glm::vec3 dir;
	dir.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	dir.y = sin(glm::radians(_pitch));
	dir.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));

	_target = glm::normalize(dir);
	_viewMatrix = glm::lookAt(_position, _position + _target, _up);
}
