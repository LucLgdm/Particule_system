/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 15:40:39 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/13 15:53:34 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ParticleSystem.hpp"

// Constructeur
ParticleSystem::ParticleSystem(size_t num, const std::string& shape): _radius(5.0f), _nbParticle(num) {
	createBuffers();			// glGenBuffers && glBufferData
	registerInterop();			// clCreateFromGLBuffer
	createKernel();				// GPU Kernel
	initializeShape(shape);		// Call the first kernel
}

ParticleSystem::~ParticleSystem() {
	if (_initShape) clReleaseKernel(_initShape);
    if (_updateSys) clReleaseKernel(_updateSys);
}

void ParticleSystem::createBuffers() {
	// Number of particles, each particle stores 4 float, a vect4(x, y, z, w)
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

	// Create context with OpenGL sharing
    cl_context_properties properties[] = {
		#ifdef __APPLE__
				CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
				(cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()),
		#elif defined(_WIN32)
				CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
				CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
				CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
		#else // Linux
				CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
				CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
				CL_CONTEXT_PLATFORM, (cl_context_properties)platform,
		#endif
				0
	};
    
    cl_int err;
    _clContext = clCreateContext(properties, 1, &device, nullptr, nullptr, &err);
    if (err != CL_SUCCESS) throw openClError("Failed to create OpenCL context with GL sharing");

	
	// Create OpenCl memory object from GL Buffers
	// This makes VRAM buffers visible to OpenCL
	_clPosBuffer = clCreateFromGLBuffer(_clContext, CL_MEM_READ_WRITE, _posBuffer, &err);
	if (err != CL_SUCCESS) throw openClError("   \033[33mFailed to create position buffer\033[0m");

	_clVelBuffer = clCreateFromGLBuffer(_clContext, CL_MEM_READ_WRITE, _velBuffer, &err);
	if (err != CL_SUCCESS) throw openClError("   \033[33mFailed to create velocity buffer\033[0m");

	_clColBuffer = clCreateFromGLBuffer(_clContext, CL_MEM_READ_WRITE, _colorBuffer, &err);
	if (err != CL_SUCCESS) throw openClError("   \033[33mFailed to create color buffer\033[0m");

	// Create the command queue: all OpenCL operations must be submitted here
	// clQueue is a mailman: aquires buffer, launches kernel and releases GL buffers
	_clQueue = clCreateCommandQueue(_clContext, device, 0, &err);
	if (err != CL_SUCCESS) throw openClError("Failed to create OpenCL command queue");

}

void ParticleSystem::createKernel() {
	// Take the .cl file
	std::ifstream file("./srcs/kernels.cl");
	 if (!file.is_open())
        throw openClError("Failed to open kernels.cl");
	
	std::string src((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
	const char* src_cstr = src.c_str();

	// Create and build the cl_program
	cl_int err;
	_clProgram = clCreateProgramWithSource(_clContext, 1, &src_cstr, nullptr, &err);
	if (err != CL_SUCCESS)
		throw openClError("   \033[33mFailed to create cl program\033[0m");

	cl_device_id device;
	clGetContextInfo(_clContext, CL_CONTEXT_DEVICES, sizeof(device), &device, nullptr);

	err = clBuildProgram(_clProgram, 1, &device, nullptr, nullptr, nullptr);
	if (err != CL_SUCCESS) {
    // Get build log
		size_t log_size;
		clGetProgramBuildInfo(_clProgram, device, CL_PROGRAM_BUILD_LOG, 0, nullptr, &log_size);
		std::vector<char> log(log_size);
		clGetProgramBuildInfo(_clProgram, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), nullptr);
		std::cerr << "Build log:\n" << log.data() << std::endl;
		
		throw openClError("   \033[33mFailed to build OpenCL program\033[0m");
	}
}

void ParticleSystem::setKernel(const std::string &shape) {
	cl_int err;
	int flag = (shape == "sphere" ? 0 : (shape == "cube" ? 1 : 2));
	// std::cout << "\033[36mSetting kernel arguments...\033[0m" << std::endl;
	// Buffer arguments
	err  = clSetKernelArg(_initShape, 0, sizeof(cl_mem), &_clPosBuffer);
	err |= clSetKernelArg(_initShape, 1, sizeof(cl_mem), &_clVelBuffer);
	err |= clSetKernelArg(_initShape, 2, sizeof(cl_mem), &_clColBuffer);
	
	// Other data
	cl_uint nb = static_cast<cl_uint>(_nbParticle); // On evite de passer un size_t* a OpenCl, il ne connait pas
	err |= clSetKernelArg(_initShape, 3, sizeof(cl_uint), &nb);
	err |= clSetKernelArg(_initShape, 4, sizeof(float), &_radius);
	err |= clSetKernelArg(_initShape, 5, sizeof(int), &flag);

	if (err != CL_SUCCESS)
		throw openClError("   \033[33mFailed to set kernel arguments\033[0m");
	
	// std::cout << "\033[32minitShape Kernel set succeffully !\033[0m" << std::endl;
}

// Aquires the OpenGl buffers for OpenCl access
// Launches an OpenCl kernel tha writes directly into OpenGl buffers
// Release the buffers back to OpenGl
void ParticleSystem::initializeShape(const std::string& shape) {
	// 1 Aquiring OpenGl buffers
	cl_int err;
	cl_mem buffer[] = {_clPosBuffer, _clVelBuffer, _clColBuffer};
	err = clEnqueueAcquireGLObjects(_clQueue, 3, buffer, 0, nullptr, nullptr);
	if (err != CL_SUCCESS)
		throw openClError("    \033[33mCan't aquire GL Objects\0330m");

	// 2 Set kernel arguments
	_initShape = clCreateKernel(_clProgram, "initShape", &err);
	if (err != CL_SUCCESS)
		throw openClError("    \033[33mFailed to create kernel _initShape\033[0m");
	
	setKernel(shape);

	_updateSys = clCreateKernel(_clProgram, "updateSpace", &err);
	if (err != CL_SUCCESS)
		throw openClError("    \033[33mFailed to create kernel\033[0m");
	// std::cout << "\033[32mudpateSpace Kernel set succeffully !\033[0m" << std::endl;

	// 3 Launch kernel
	clEnqueueNDRangeKernel(_clQueue, _initShape, 1,
		nullptr, &_nbParticle, nullptr, 0, nullptr, nullptr);
	
	// 4 Release buffers back to OpenGl
	clEnqueueReleaseGLObjects(_clQueue, 3, buffer, 0, nullptr, nullptr);

	// 5. Ensure completion before rendering
	clFinish(_clQueue);
}

void ParticleSystem::setupRendering() {
	// Create VAO
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	
	// Bind position buffer
	glBindBuffer(GL_ARRAY_BUFFER, _posBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);
	
	// Bind color buffer
	glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);
	
	glBindVertexArray(0);
}

void ParticleSystem::render() {
	glBindVertexArray(_vao);
	glDrawArrays(GL_POINTS, 0, _nbParticle);
	glBindVertexArray(0);
}

void ParticleSystem::acquireGLObjects() {
	cl_mem buffers[] = {_clPosBuffer, _clVelBuffer, _clColBuffer};
	clEnqueueAcquireGLObjects(_clQueue, 3, buffers, 0, nullptr, nullptr);
}

void ParticleSystem::releaseGLObjects() {
	cl_mem buffers[] = {_clPosBuffer, _clVelBuffer, _clColBuffer};
	clEnqueueReleaseGLObjects(_clQueue, 3, buffers, 0, nullptr, nullptr);
	clFlush(_clQueue); // push les commandes ; évite blocage implicite côté GL
}

void ParticleSystem::update(float dt) {	
	// 1 Aquiring OpenGl buffers
	cl_int err;
	cl_mem buffers[] = {_clPosBuffer, _clVelBuffer, _clColBuffer};
	err = clEnqueueAcquireGLObjects(_clQueue, 3, buffers, 0, nullptr, nullptr);
	if (err != CL_SUCCESS) throw openClError("Can't acquire GL objects");

	// 2 Set kernel arguments
	err  = clSetKernelArg(_updateSys, 0, sizeof(cl_mem), &_clPosBuffer);
	err |= clSetKernelArg(_updateSys, 1, sizeof(cl_mem), &_clVelBuffer);
	err |= clSetKernelArg(_updateSys, 2, sizeof(cl_mem), &_clColBuffer);
	cl_uint nb = static_cast<cl_uint>(_nbParticle);
	err |= clSetKernelArg(_updateSys, 3, sizeof(cl_uint), &nb);
	err |= clSetKernelArg(_updateSys, 4, sizeof(float), &dt);
	// cl_float4 gravity = {0.0f, -9.8f, 0.0f, 0.0f};
	// err |= clSetKernelArg(_updateSys, 4, sizeof(cl_float4), &gravity);
	cl_float4 gravityPos = {0.9f, 0.0f, 0.0f, 0.0f};
	float gravityMass = 1.0f * 50.f;
	err |= clSetKernelArg(_updateSys, 5, sizeof(cl_float4), &gravityPos);
	err |= clSetKernelArg(_updateSys, 6, sizeof(float), &gravityMass);
	err |= clSetKernelArg(_updateSys, 7, sizeof(int), &_gravityEnable);
	if (err != CL_SUCCESS) throw openClError("Failed to set kernel arguments");

	// 3 Launch kernel
	size_t local = 128;
	size_t global = ((static_cast<size_t>(_nbParticle) + local - 1) / local) * local;
	err = clEnqueueNDRangeKernel(_clQueue, _updateSys, 1, nullptr, &global, &local, 0, nullptr, nullptr);
	if (err != CL_SUCCESS) throw openClError("Failed to enqueue kernel");

	// 4 Release buffers back to OpenGl and Flush
	clEnqueueReleaseGLObjects(_clQueue, 3, buffers, 0, nullptr, nullptr);
	clFlush(_clQueue);
}
