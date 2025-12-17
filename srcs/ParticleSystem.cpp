/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 15:40:39 by lde-merc          #+#    #+#             */
/*   Updated: 2025/12/17 16:46:38 by lde-merc         ###   ########.fr       */
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

// The GPU buffers becomes visible to OpenCL for computation
// Critical step for performance
// Acquire and release buffer
// Avoid CPU-side copies
void ParticleSystem::registerInterop() {
	// Create the context in which OpenCl will evolve
	cl_platform_id platform;
	clGetPlatformIDs(1, &platform, nullptr);

	// Which GPU
	cl_device_id device;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, nullptr);

	cl_int err;
	_context = clCreateContext(nullptr, 1, &device, nullptr, nullptr, &err);
	
	// Create OpenCl memory object from GL Buffers
	// This makes VRAM buffers visible to OpenCL
	_clPosBuffer = clCreateFromGLBuffer(_context, CL_MEM_READ_WRITE, _posBuffer, &err);
	if (err != CL_SUCCESS) throw openClError("   \033[33mFailed to create position buffer\033[0m");

	_clVelBuffer = clCreateFromGLBuffer(_context, CL_MEM_READ_WRITE, _velBuffer, &err);
	if (err != CL_SUCCESS) throw openClError("   \033[33mFailed to create velocity buffer\033[0m");

	_clColBuffer = clCreateFromGLBuffer(_context, CL_MEM_READ_WRITE, _colorBuffer, &err);
	if (err != CL_SUCCESS) throw openClError("   \033[33mFailed to create color buffer\033[0m");

	// Create the command queue: all OpenCL operations must be submitted here
	// clQueue is a mailman: aquires buffer, launches kernel and releases GL buffers
	_clQueue = clCreateCommandQueue(_clContext, device, 0, &err);
	if (err != CL_SUCCESS) throw openClError("Failed to create OpenCL command queue");

}

void ParticleSystem::initializeSphere() {
	
}

void ParticleSystem::initializeCube() {
	
}

