/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParticleSystem.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 15:40:34 by lde-merc          #+#    #+#             */
/*   Updated: 2025/12/17 16:13:04 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class ParticleSystem {
	public:
		ParticleSystem(size_t, const std::string&);
		~ParticleSystem();
		
		ParticleSystem(const ParticleSystem &other) = delete;
		ParticleSystem &operator=(const ParticleSystem &other) = delete;

		void createBuffers();
		void registerInterop();
		void initializeSphere();
		void initializeCube();
		
		void update(float dt);

		GLuint posBuffer() const { return _posBuffer; };
		GLuint velBuffer() const { return _velBuffer; };
		GLuint colBuffer() const { return _colorBuffer; };
		
	private:
		size_t _nbParticle;

		GLuint _posBuffer;
		GLuint _velBuffer;
		GLuint _colorBuffer;
		
};
