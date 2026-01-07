/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:42:47 by lde-merc          #+#    #+#             */
/*   Updated: 2026/01/07 13:40:36 by lde-merc         ###   ########.fr       */
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
	_lastFpsTime = 0.0f;
	
	initGLFW();
	initOpenGL();
	_system = std::make_unique<ParticleSystem>(_nbParticle, _shape);
	_system->setupRendering();

	initShader();
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
	glfwSwapInterval(1);
}

void Application::initOpenGL() {
	// Charger les fonctions OpenGL avec Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw openGlError("   \033[33mFailed to initialize GLAD\033[0m");
	
	// Configuration OpenGL
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE);
	// glEnable(GL_CULL_FACE);
	// glDisable(GL_CULL_FACE);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

static std::string readFile(const char* path) {
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error(std::string("Cannot open shader file: ") + path);

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Application::initShader() {
	const char* vPath = "shaders/vertex.glsl";
	const char* fPath = "shaders/fragment.glsl";
	
	std::string vCode = readFile(vPath);
	std::string fCode = readFile(fPath);
	
	const char* vSrc = vCode.c_str();
	const char* fSrc = fCode.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vSrc, nullptr);
	glCompileShader(vShader);

	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fSrc, nullptr);
	glCompileShader(fShader);
	
	_shaderProgram = glCreateProgram();

	glAttachShader(_shaderProgram, vShader);
	glAttachShader(_shaderProgram, fShader);
	glLinkProgram(_shaderProgram);

	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

void Application::cleanup() {
	if (_shaderProgram) glDeleteProgram(_shaderProgram);
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void Application::run() {
	_lastFrameTime = glfwGetTime();
	_lastFpsTime = _lastFrameTime;
	
	while (!glfwWindowShouldClose(_window)) {
		float currentTime = glfwGetTime();
		// dt OpenCl
		float dt = currentTime - _lastFrameTime;
		dt = std::min(dt, 0.02f);
		_lastFrameTime = currentTime;
		
		// Fps
		_fps++;
        if (currentTime - _lastFpsTime >= 1.0)
        {
            double fps = _fps / (currentTime - _lastFpsTime);

            std::string title =
                "Particule system | FPS: " + std::to_string(int(fps));
            glfwSetWindowTitle(_window, title.c_str());

            _fps  = 0;
            _lastFpsTime = currentTime;
        }
		
		// 1. OpenCL écrit → OpenGL lit
		// glFinish();
		// _system->acquireGLObjects();	// clEnqueueAcquireGLObjects
		// _system->update(dt);			// kernels OpenCL
		// _system->releaseGLObjects();	// clEnqueueReleaseGLObjects
		
		/* TEST */
		_system->update(dt);
		/* FIN TEST */
		
		// 2. OpenGL rend

		float aspect = (float)WIDTH / (float)HEIGHT;

		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			aspect,
			0.1f,
			100.0f
		);

		glm::mat4 view = glm::lookAt(
			glm::vec3(0.0f, 0.0f, 3.0f),  // caméra
			glm::vec3(0.0f, 0.0f, 0.0f),  // centre
			glm::vec3(0.0f, 1.0f, 0.0f)   // haut
		);

		glm::mat4 model = glm::mat4(1.0f);

		glm::mat4 mvp = projection * view * model;
		
		glUseProgram(_shaderProgram);
		
		GLuint uMVP = glGetUniformLocation(_shaderProgram, "uMVP");
		glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		_system->render();
		glFlush();

		glfwSwapBuffers(_window);
		glfwPollEvents();

	}
}

