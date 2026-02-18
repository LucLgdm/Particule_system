/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/17 13:42:47 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/18 16:30:22 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Application.hpp"

static bool hPressed = true;

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
	_imguiLayer.initImGui(_window);
}

void Application::checkinput(int argc, char **argv) {
	if (argc != 3){
		ostringstream oss;
		oss << "   The program needs 2 arguments: " << std::endl;
		oss << "      \033[33m_the number of particle" << std::endl;
		oss << "      _the shape (sphere or cube)" << std::endl;
		oss << "	  Everything can be change while playing!\033[0m" << std::endl;
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

	setCallbacks();
}

void Application::setCallbacks() {
	// Scroll callback for zooming
	glfwSetWindowUserPointer(_window, this);
	glfwSetScrollCallback(_window, [](GLFWwindow* win, double xoffset, double yoffset) {
		Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(win));
		if (app->_cameraMode == CameraMode::FPS)
			app->_cameraFps.processScroll(static_cast<float>(yoffset));
		else
			app->_cameraOrbit.processScroll(static_cast<float>(yoffset));
	});

	// Callbacks for Camera rotation
	glfwSetMouseButtonCallback(_window, [](GLFWwindow* win, int button, int action, int mods) {
		Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(win));
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (app->_cameraMode == CameraMode::FPS) {
				if (action == GLFW_PRESS)
					app->_cameraFps.beginRotate();
				else if (action == GLFW_RELEASE)
					app->_cameraFps.endRotate();
			} else {
				if (action == GLFW_PRESS)
					app->_cameraOrbit.beginRotate();
				else if (action == GLFW_RELEASE)
					app->_cameraOrbit.endRotate();
			}
		}
	});

	glfwSetCursorPosCallback(_window, [](GLFWwindow* win, double xpos, double ypos) {
		Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(win));
		if (app->_cameraMode == CameraMode::FPS)
			app->_cameraFps.processMouseMove(static_cast<float>(xpos),
								static_cast<float>(ypos));
		else
			app->_cameraOrbit.processMouseMove(static_cast<float>(xpos),
								static_cast<float>(ypos));
	});
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
	_axisGizmo.cleanup();
	if (_shaderProgram) glDeleteProgram(_shaderProgram);
	glfwDestroyWindow(_window);
	glfwTerminate();
}

void Application::run() {
	_lastFrameTime = glfwGetTime();
	_lastFpsTime = _lastFrameTime;
	_axisGizmo.init(_shaderProgram);
	
	while (!glfwWindowShouldClose(_window)) {
		float currentTime = glfwGetTime();
		// dt OpenCl
		float dt = currentTime - _lastFrameTime;
		dt = std::min(dt, 0.02f);
		_lastFrameTime = currentTime;
		
		// Fps
		handleFps();
		
		handleKey();
		
		// 1. OpenCL écrit → OpenGL lit
		_system->update(dt);
		
		// 2. OpenGL rend
		updateCam();

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = getProjectionMatrix() * getViewMatrix() * model;
		glUseProgram(_shaderProgram);
		
		GLuint uMVP = glGetUniformLocation(_shaderProgram, "uMVP");
		glUniformMatrix4fv(uMVP, 1, GL_FALSE, glm::value_ptr(mvp));
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		_system->render();
		glFlush();
		
		_axisGizmo.render(getViewMatrix(), getProjectionMatrix(), 0.1f, glm::vec3(0.0f, 0.0f, 0.0f));
		
		if (hPressed) {	
			_imguiLayer.beginFrame();
			_imguiLayer.render(*_system, _cameraMode, _cameraOrbit);
			_imguiLayer.endFrame();
		}

		glfwSwapBuffers(_window);
		glfwPollEvents();

	}
	_imguiLayer.shutdown();
}

void Application::handleFps() {
	_fps++;
	float currentTime = glfwGetTime();
	if (currentTime - _lastFpsTime >= 1.0)
	{
		double fps = _fps / (currentTime - _lastFpsTime);

		std::string title =
			"Particule system | FPS: " + std::to_string(int(fps));
		glfwSetWindowTitle(_window, title.c_str());

		_fps  = 0;
		_lastFpsTime = currentTime;
	}
}

void Application::handleKey() {
	
	// --- Fullscreen toggle ---
	static bool f11Pressed = false;
	bool f11Now = (glfwGetKey(_window, GLFW_KEY_F11) == GLFW_PRESS);
	if (f11Now && !f11Pressed) {
		toggleFullscreen();   // ta fonction qui gère le passage full screen
	}
	f11Pressed = f11Now;

	// --- UI toggle ---
	bool hNow = (glfwGetKey(_window, GLFW_KEY_H) == GLFW_PRESS);
	static bool hHandled = false;
	if (hNow && !hHandled) {
		hPressed = !hPressed;
		hHandled = true;
	} else if (!hNow) {
		hHandled = false;
	}

	// --- Escape to quit if not in fullscreen mode ---
	static bool escPressed = false;
	bool escNow = (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS);

	if (escNow && !escPressed) {
		GLFWmonitor* monitor = glfwGetWindowMonitor(_window);

		if (monitor) {
			toggleFullscreen();
		} else {
			glfwSetWindowShouldClose(_window, true);
		}
	}
	escPressed = escNow;

	handleMouseGravityPoint();
}

void Application::toggleFullscreen() {
	_fullscreen = !_fullscreen;
	
	if (_fullscreen) {
		// Save position and size of the window
		glfwGetWindowPos(_window, &_windowedX, &_windowedY);
		glfwGetWindowSize(_window, &_windowedWidth, &_windowedHeight);
		
		// Fullscreen
		_currentMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(_currentMonitor);
		_currentWidth = mode->width;
		_currentHeight = mode->height;
		
		glfwSetWindowMonitor(_window, _currentMonitor, 0, 0, mode->width,
				mode->height, mode->refreshRate);

		// Update viewport and camera aspect ratio
		glViewport(0, 0, mode->width, mode->height);
		_cameraFps.updateProjectionMatrix(mode->width, mode->height); // Reset camera to update aspect ratio
		_cameraOrbit.updateProjectionMatrix(mode->width, mode->height); // Reset camera to update aspect ratio
	} else {
		// Back to windowed mode
		_currentWidth = _windowedWidth;
		_currentHeight = _windowedHeight;
		
		glfwSetWindowMonitor(_window, nullptr, _windowedX, _windowedY, WIDTH, HEIGHT, 0);

		// Update viewport and camera aspect ratio
		glViewport(0, 0, _windowedWidth, _windowedHeight);
		_cameraFps.updateProjectionMatrix(_windowedWidth, _windowedHeight); // Reset camera to update aspect ratio
		_cameraOrbit.updateProjectionMatrix(_windowedWidth, _windowedHeight); // Reset camera to update aspect ratio
	}
}

void Application::handleMouseGravityPoint() {
	static bool mPressed = false;

	if (glfwGetKey(_window, GLFW_KEY_M) == GLFW_PRESS) {
		if (!mPressed) {
			mPressed = true;

			if (_mouseGravityIndex == -1) {
				// Crée un nouveau point de gravité
				float d = glm::distance(_cameraOrbit.getPosition(), _cameraOrbit.getTarget());
				glm::vec3 forward = glm::normalize(_cameraOrbit.getTarget() - _cameraOrbit.getPosition());
				glm::vec3 refPoint = _cameraOrbit.getPosition() + forward * d;

				float mx = _cameraOrbit.getLastX();
				float my = _cameraOrbit.getLastY();
				float nx = (2.0f * mx) / WIDTH - 1.0f;
				float ny = 1.0f - (2.0f * my) / HEIGHT;

				glm::vec4 clipCoords(nx, ny, -1.0f, 1.0f);
				glm::vec4 eyeCoords = glm::inverse(_cameraOrbit.getProjectionMatrix()) * clipCoords;
				eyeCoords.z = -1.0f; eyeCoords.w = 0.0f;

				glm::vec3 rayDir = glm::normalize(glm::vec3(glm::inverse(_cameraOrbit.getViewMatrix()) * eyeCoords));
				glm::vec3 rayOrigin = _cameraOrbit.getPosition();

				float denom = glm::dot(forward, rayDir);
				if (fabs(denom) > 0.0001f) {
					float t = glm::dot(refPoint - rayOrigin, forward) / denom;
					glm::vec3 intersectPos = rayOrigin + t * rayDir;

					// On ajoute le GravityPoint et on garde son index
					std::vector<GravityPoint> gp = _system->getGravityPoint();
					int type = gp[0]._type;
					_system->addGravityPoint(intersectPos.x, intersectPos.y, intersectPos.z, 300.0f, true, type);
					_mouseGravityIndex = _system->getNGravityPos() - 1;
				}
			} else {
				// Supprime le point existant
				_system->removeGravityPoint(_mouseGravityIndex);
				_mouseGravityIndex = -1;
			}
		}
	} else {
		mPressed = false; // reset du toggle
	}

	// If the gp exists, it has to follow the mouse 
	if (_mouseGravityIndex != -1) {
		float d = glm::distance(_cameraOrbit.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f));
		glm::vec3 forward = glm::normalize(_cameraOrbit.getTarget() - _cameraOrbit.getPosition());
		glm::vec3 refPoint = _cameraOrbit.getPosition() + forward * d;

		float mx = _cameraOrbit.getLastX();
		float my = _cameraOrbit.getLastY();
		float nx = (2.0f * mx) / _currentWidth - 1.0f;
		float ny = 1.0f - (2.0f * my) / _currentHeight;

		glm::vec4 clipCoords(nx, ny, -1.0f, 1.0f);
		glm::vec4 eyeCoords = glm::inverse(_cameraOrbit.getProjectionMatrix()) * clipCoords;
		eyeCoords.z = -1.0f; eyeCoords.w = 0.0f;

		glm::vec3 rayDir = glm::normalize(glm::vec3(glm::inverse(_cameraOrbit.getViewMatrix()) * eyeCoords));
		glm::vec3 rayOrigin = _cameraOrbit.getPosition();

		float denom = glm::dot(forward, rayDir);
		if (fabs(denom) > 0.0001f) {
			float t = glm::dot(refPoint - rayOrigin, forward) / denom;
			glm::vec3 intersectPos = rayOrigin + t * rayDir;

			// Upadating the position
			_system->updatePositionGP(_mouseGravityIndex, intersectPos.x, intersectPos.y, intersectPos.z, 400.0f);
		}
	}
}