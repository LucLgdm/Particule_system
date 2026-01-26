# Copilot Instructions for Particle System

## Project Architecture

**Particle System** is a GPU-accelerated particle simulation engine using OpenGL, OpenCL, and ImGui. It simulates particles under gravity with interactive 3D visualization.

### Core Components

1. **Application** ([Application.hpp](../includes/Application.hpp))
   - Main entry point managing GLFW window, OpenGL context, and main loop
   - Owns ParticleSystem and ImGuiLayer instances
   - Handles camera switching (FPS vs Orbit modes)
   - Delegates rendering via `render()` method

2. **ParticleSystem** ([ParticleSystem.hpp](../includes/ParticleSystem.hpp))
   - GPU memory management (position, velocity, color buffers via OpenGL)
   - OpenCL-OpenGL interop for compute shader integration
   - Gravity simulation via OpenCL kernels
   - Must NOT use std::vector for particle data—all CPU-side allocation forbidden
   - Creates buffers with `GL_DYNAMIC_DRAW` for GPU-only data

3. **ImGuiLayer** ([ImGuiLayer.hpp](../includes/ImGuiLayer.hpp))
   - Debug UI for real-time gravity point control
   - Displays particle stats and simulation parameters
   - Supports two camera modes via enum `CameraMode::{ORBIT, FPS}`

4. **Camera Systems**
   - **CameraFps**: First-person view with WASD movement
   - **CameraOrbit**: Third-person orbit around particle center
   - Both implement `getViewMatrix()` and `getProjectionMatrix()`

## Data Flow & Synchronization

```
ParticleSystem Buffers (GPU)
  ├─ posBuffer (GL_COPY_READ_BUFFER)
  ├─ velBuffer
  └─ colorBuffer
         ↓
OpenCL Kernel (kernels.cl)
  └─ Updates physics via gravity computation
         ↓
Acquire/Release GL Objects
  └─ acquireGLObjects() / releaseGLObjects()
         ↓
Rendering (Vertex Shader → Fragment Shader)
```

**Critical Pattern**: Before any OpenCL kernel execution, call `acquireGLObjects()`. After kernels complete, call `releaseGLObjects()`. This prevents device resource conflicts.

## Key Workflows

### Build
```bash
make       # Compile with g++ -std=c++17, links libOpenCL, GLFW, OpenGL
make clean # Remove object files
make fclean # Remove all artifacts
make re    # Full rebuild
make val   # Run with valgrind leak checker
```

### Adding New Features

**Kernel Extension**: Edit [kernels.cl](../srcs/kernels.cl)
- Uses Fibonacci sphere generation for initial distribution
- Hash function for pseudo-random particle initialization
- Gravity struct mirrors C++ `GravityPoint` (must stay synchronized)

**Shader Changes**: Edit [vertex.glsl](../shaders/vertex.glsl) and [fragment.glsl](../shaders/fragment.glsl)
- Vertex shader applies MVP matrix; Fragment shader outputs color
- Point rendering with `gl_PointSize = 2.0`

**UI Controls**: Edit [ImGuiLayer.cpp](../srcs/ImGuiLayer.cpp)
- Use `renderPS()` for particle system parameters
- Use `renderCamera()` for camera controls

## Project Conventions

### Error Handling
Custom exceptions in [exception.hpp](../includes/exception.hpp):
- `inputError` – command-line argument issues
- `glfwError` – window/context failures
- `openGlError` – shader/buffer creation failures
- `openClError` – kernel compilation or device errors

Catch exceptions in `main()` with colored stderr output.

### Naming Conventions
- Private members prefixed with `_` (e.g., `_nbParticle`, `_posBuffer`)
- Classes use PascalCase; methods use camelCase
- OpenGL handles use `GL*` (e.g., `GLuint`, `GLFWwindow*`)
- OpenCL handles use `cl_*` (e.g., `cl_float4`, `cl_command_queue`)

### Memory Model
- **No std::vector for particle data** – breaks GPU memory isolation
- Use explicit OpenGL buffer creation: `glGenBuffers()`, `glBindBuffer()`, `glBufferData()`
- All particle initialization happens in OpenCL, never on CPU

### Gravity System
- `GravityPoint` struct holds mass and position (4-element vector for alignment)
- Gravity disabled by default (`_gravityEnable = 0`)
- Dynamic gravity point management via `addGravityPoint()` and `removeGravityPoint()`
- Sync CPU gravity array to GPU via `updateGravityBuffer()`

## Dependencies & Integration

| Dependency | Role | Location |
|---|---|---|
| OpenGL 3.3+ | Rendering | glad, GLM |
| GLFW | Window/Input | system library |
| OpenCL 1.2+ | GPU Compute | libOpenCL.so.1 |
| GLM | Matrix Math | `includes/glm/` |
| ImGui | Debug UI | `includes/` + `srcs/imGui/` |

**Critical Link Flags** (from Makefile):
```
-l:libOpenCL.so.1 -lglfw -lGL
```

## Testing & Debugging

- **Shader Errors**: Check console output; shader compilation errors printed to stderr
- **OpenCL Errors**: Wrap kernel builds in try-catch for `openClError`
- **Memory Leaks**: Run `make val` to inspect with valgrind
- **Visual Debugging**: Toggle gravity on/off in ImGui to verify kernel execution
