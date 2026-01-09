#include "Camera.hpp"

// Constructeur
Camera::Camera() {}

Camera::~Camera() {}

Camera::Camera(const Camera &other) {
    *this = other;
}

Camera &Camera::operator=(const Camera &other) {
    if (this != &other) {
        // copy attributes here
    }
    return *this;
}
