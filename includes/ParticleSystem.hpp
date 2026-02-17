/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 15:40:34 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/17 14:19:08 by lde-merc         ###   ########.fr       */
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
	uint32_t _active; // 1 active, -1 inactive
	int _type; // 0 gravity, 1 Lorentz, 2 Curl noise, 3 repulsion

	GravityPoint();
	GravityPoint(float x, float y, float z, float w, float m)
		: _Position{ x, y, z, w }, _Mass(m), _active(0), _type(0) {}

	float getx() const { return _Position.s[0]; };
	float gety() const { return _Position.s[1]; };
	float getz() const { return _Position.s[2]; };
	bool getState() const { return _active; };
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
		void releaseBuffers();
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

		float getRadius() const {return _radius; };
		void setRadius(float r) { _radius = r; };
		int getShape() const {return _shape; };
		
		void setGravity(bool);
		void setSpeed(int speed) { _speed = speed; };
		void setType(int);

		size_t getNPart() const { return _nbParticle; };
		void setNbPart(int);

		int& getColorMode() { return _colorMode; };
		void setColorMode(int mode) { _colorMode = mode; };

		void addGravityPoint(float, float, float, float, bool, int);
		void removeGravityPoint(int);
		
		void updateGravityBuffer();
		
	private:
		int _shape; // 0 sphere, 1 cube, 2 pyramid
		size_t _nbParticle;
		float _radius;
		int _gravityEnable = 0;
		int _nGravityPos = 0;
		int _colorMode = 0;
		int _speed = 0;
		float _time = 0.0f;

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
