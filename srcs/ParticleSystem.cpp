/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 15:40:39 by lde-merc          #+#    #+#             */
/*   Updated: 2025/12/17 16:25:07 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParticleSystem.hpp"

// Constructeur
ParticleSystem::ParticleSystem(size_t num, const std::string& shape): _nbParticle(num) {
	createBuffers();			// glGenBuffers && glBufferData
	registerInterop();		// clCreateFromGLBuffer
	if (shape == "sphere")	// GPU Kernel
		initializeSphere();
	else
		initializeCube();
}

ParticleSystem::~ParticleSystem() {}

void ParticleSystem::createBuffers() {
	const std::size_t bufferSize = _nbParticle * sizeof(float) * 4;
	
	glGenBuffers(1, &_posBuffer); 				// 1 is for the number of buffer object
	glBindBuffer(GL_ARRAY_BUFFER, _posBuffer);	// Vertex attributes
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
	// nullptr is the proof that no CPU memory is used here
	// GL_DYNAMIC_DRAW because, it's updated every frame: OpenGl drivers treats this as "frequently modified"

	glGenBuffers(1, &_velBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _velBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleSystem::registerInterop() {
	
}

void ParticleSystem::initializeSphere() {
	
}

void ParticleSystem::initializeCube() {
	
}

