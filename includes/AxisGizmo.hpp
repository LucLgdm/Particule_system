/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AxisGizmo.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 12:14:32 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/26 12:27:11 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


class AxisGizmo {
	public:
		AxisGizmo();
		~AxisGizmo();
		
		void init(GLuint shaderProgram);
		void render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, 
					float scale = 2.0f, glm::vec3 position = glm::vec3(0.0f));
		void cleanup();

	private:
		GLuint _vao, _vbo, _ebo;
		GLuint _shaderProgram;
		size_t _indexCount;
	
};
