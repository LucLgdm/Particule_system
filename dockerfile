FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    # OpenGL
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    # OpenCL
    opencl-headers \
    ocl-icd-opencl-dev \
    clinfo \
    # GLFW / GLEW (souvent utiles avec OpenGL)
    libglfw3-dev \
    libglew-dev \
    # Utilitaires
    pkg-config \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /project

CMD ["/bin/bash"]