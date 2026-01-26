/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 15:40:34 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/26 17:39:40 by lde-merc         ###   ########.fr       */
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

#include <GL/glx.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "exception.hpp"


struct GravityPoint {
	float _Mass;
	cl_float4 _Position;
	uint32_t active; // 1 active, -1 inactive

	GravityPoint();
	GravityPoint(float x, float y, float z, float w, float m)
		: _Position{ x, y, z, w }, _Mass(m), active(0) {}

	float getx() const { return _Position.s[0]; };
	float gety() const { return _Position.s[1]; };
	float getz() const { return _Position.s[2]; };
	bool getState() const { return active; };
	float getMass() const { return _Mass; };

	void setPos(float pos[4]) {
		_Position.s[0] = pos[0];
		_Position.s[1] = pos[1];
		_Position.s[2] = pos[2];
		_Mass = pos[3];
	}
	
};


class ParticleSystem {
	public:
		ParticleSystem(size_t, const std::string&);
		~ParticleSystem();
		
		ParticleSystem(const ParticleSystem &other) = delete;
		ParticleSystem &operator=(const ParticleSystem &other) = delete;

		void createBuffers();
		void registerInterop();
		void createKernel();
		void setKernel(const std::string &);
		void initializeShape(const std::string &);
		
		void setupRendering();
		void render();

		void acquireGLObjects();
		void releaseGLObjects();
		void update(float dt);

		GLuint posBuffer() const { return _posBuffer; };
		GLuint velBuffer() const { return _velBuffer; };
		GLuint colBuffer() const { return _colorBuffer; };
		int getNGravityPos() const { return _nGravityPos; };
		const std::vector<GravityPoint>& getGravityPoint() const { return _GravityCenter; }
		std::vector<GravityPoint>& getGravityPoint() { return _GravityCenter; }

		void setGravity(bool);

		bool& getColorMode() { return _colorMode; };
		void setColorMode(bool mode) { _colorMode = mode; };

		void addGravityPoint(float, float, float, float);
		void removeGravityPoint(int);
		
		void updateGravityBuffer();
		
	private:
		size_t _nbParticle;
		float _radius;
		int _gravityEnable = 0;
		int _nGravityPos = 0;
		bool _colorMode = false;

		std::vector<GravityPoint> _GravityCenter;

		// OpenGl
		GLuint _posBuffer;
		GLuint _velBuffer;
		GLuint _colorBuffer;
		GLuint _vao;
		
		// OpenCl
		cl_context _clContext;
		cl_command_queue _clQueue;
		cl_program _clProgram;
			// memory
		cl_mem _clPosBuffer;
		cl_mem _clVelBuffer;
		cl_mem _clColBuffer;
		cl_mem _clGravityBuffer = nullptr;
			// kernel
		cl_kernel _initShape;
		cl_kernel _updateSys;
};
