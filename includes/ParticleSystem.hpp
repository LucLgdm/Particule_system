/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 15:40:34 by lde-merc          #+#    #+#             */
/*   Updated: 2025/12/18 13:37:28 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CL_TARGET_OPENCL_VERSION 120

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// OpenCl
#include <CL/cl.h>
#include <CL/cl_gl.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <string>
#include <iostream>
#include <fstream>
#include "exception.hpp"

class ParticleSystem {
	public:
		ParticleSystem(size_t, const std::string&);
		~ParticleSystem();
		
		ParticleSystem(const ParticleSystem &other) = delete;
		ParticleSystem &operator=(const ParticleSystem &other) = delete;

		void createBuffers();
		void registerInterop();
		void createKernel();
		void setKernel();
		void initializeSphere();
		void initializeCube();
		
		void update(float dt);

		GLuint posBuffer() const { return _posBuffer; };
		GLuint velBuffer() const { return _velBuffer; };
		GLuint colBuffer() const { return _colorBuffer; };
		
	private:
		size_t _nbParticle;
		float _radius;


		// OpenGl
		GLuint _posBuffer;
		GLuint _velBuffer;
		GLuint _colorBuffer;

		// OpenCl
		cl_context _clContext;
		cl_command_queue _clQueue;
		cl_program _clProgram;
			// memory
		cl_mem _clPosBuffer;
		cl_mem _clVelBuffer;
		cl_mem _clColBuffer;
			// kernel
		cl_kernel _initSphereKernel;
};
