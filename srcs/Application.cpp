/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:42:47 by lde-merc          #+#    #+#             */
/*   Updated: 2025/12/17 15:55:19 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Application.hpp"

// Constructeur
Application::Application() {}

Application::~Application() {
	cleanup();
}

Application::Application(const Application &other) {
    *this = other;
}

Application &Application::operator=(const Application &other) {
    if (this != &other) {
        this->_window = other._window;
    }
    return *this;
}

void Application::init(int argc, char **argv) {
	checkinput(argc, argv);
	
	_system = std::make_unique<ParticleSystem>(_nbParticle, _shape);
	
	initGLFW();
	initOpenGL();
}

void Application::checkinput(int argc, char **argv) {
	if (argc != 3){
		ostringstream oss;
		oss << "   The program needs 2 arguments: " << std::endl;
		oss << "      \033[33m_the number of particle" << std::endl;
		oss << "      _the shape (sphere or cube)\033[0m" << std::endl;
		throw inputError(oss.str());
	}

	try {
        _nbParticle = std::stoi(argv[1]);
    } catch (const std::invalid_argument&) {
        throw inputError("\033[33m   The number of particles is not a valid integer.\033[0m");
    } catch (const std::out_of_range&) {
        throw inputError("\033[33m   The number of particles is out of range.\033[0m");
    }
	if (_nbParticle <= 0)
		throw inputError("\033[33m   Warning, the number must be positiv strict !\033[0m");		

	_shape = std::string(argv[2]);
	if (_shape != "sphere" && _shape != "cube")
		throw inputError("\033[33m   Warning, the shpe must be 'sphere' or 'cube' !\033[0m");
}

void Application::initGLFW() {
	if (!glfwInit())
		throw glfwError("GLFW initialization failed");
		
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	_window = glfwCreateWindow(WIDTH, HEIGHT, "Particule_system", nullptr, nullptr);
	if (!_window)
		throw glfwError("Window creation failed");
	
	glfwMakeContextCurrent(_window);
}

void Application::initOpenGL() {
	// Charger les fonctions OpenGL avec Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw openGlError("Failed to initialize GLAD");
	
	// Configuration OpenGL
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	// glEnable(GL_CULL_FACE);
	// glDisable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void Application::cleanup() {
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void Application::run() {
	while (!glfwWindowShouldClose(_window)) {
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glfwSwapBuffers(_window);
		glfwPollEvents();

	}
}

