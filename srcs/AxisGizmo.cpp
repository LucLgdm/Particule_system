/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AxisGizmo.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 12:15:33 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/26 12:27:07 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AxisGizmo.hpp"
#include <iostream>


AxisGizmo::AxisGizmo() : _vao(0), _vbo(0), _ebo(0), _indexCount(0), _shaderProgram(0) {}

AxisGizmo::~AxisGizmo() {
	cleanup();
}

void AxisGizmo::init(GLuint shaderProgram) {
	_shaderProgram = shaderProgram;
	
	// Vertices: Position (3) + Color (3)
	float vertices[] = {
		// X axis (Red)
		0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
		100.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
		
		// Y axis (Green)
		0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,
		0.0f, 100.0f, 0.0f,    0.0f, 1.0f, 0.0f,
		
		// Z axis (Blue)
		0.0f, 0.0f, 0.0f,    0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 100.0f,    0.0f, 0.0f, 1.0f,
	};
	
	unsigned int indices[] = {
		0, 1,  // X axis
		2, 3,  // Y axis
		4, 5   // Z axis
	};
	
	_indexCount = 6;
	
	// Création VAO/VBO/EBO
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);
	glGenBuffers(1, &_ebo);
	
	glBindVertexArray(_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// Position attribute (location 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	// Color attribute (location 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void AxisGizmo::render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix,
					float scale, glm::vec3 position) {
	glUseProgram(_shaderProgram);
	
	// Utiliser la caméra du monde réel
	// Matrice modèle : position dans le monde + scale
	glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
	model = glm::scale(model, glm::vec3(scale));
	
	// Calculer MVP avec la vraie caméra du monde
	glm::mat4 mvp = projectionMatrix * viewMatrix * model;
	
	// Passer MVP au shader
	GLint mvpLoc = glGetUniformLocation(_shaderProgram, "uMVP");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	
	// Désactiver le test de profondeur pour que les axes soient toujours visibles
	glDisable(GL_DEPTH_TEST);
	
	// Rendre les axes
	glBindVertexArray(_vao);
	glLineWidth(2.5f);
	glDrawElements(GL_LINES, _indexCount, GL_UNSIGNED_INT, 0);
	glLineWidth(1.0f);
	glBindVertexArray(0);
	
	// Réactiver le test de profondeur
	glEnable(GL_DEPTH_TEST);
}

void AxisGizmo::cleanup() {
	if (_vao) glDeleteVertexArrays(1, &_vao);
	if (_vbo) glDeleteBuffers(1, &_vbo);
	if (_ebo) glDeleteBuffers(1, &_ebo);
}
