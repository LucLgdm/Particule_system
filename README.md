# 🚀 Particle System

Un moteur de simulation de particules **haute-performance** utilisant **OpenGL**, **OpenCL** et **ImGui**. Simule des milliers de particules soumises à la gravité avec rendu 3D temps réel et interface de contrôle interactive.

> **Langage** : C++  
> **État** : Stable  
> **Dernière mise à jour** : February 2026

---

## ✨ Caractéristiques

- GPU Compute : calculs physiques parallèles via OpenCL
- Rendu 3D temps réel : pipeline OpenGL 3.3+ avec shaders GLSL
- Caméras multiples : modes FPS et Orbit commutables à la volée
- Interface ImGui : ajout et configuration de points de gravité en temps réel
- Interopérabilité OpenGL / OpenCL : buffers partagés et synchronisation explicite
- Simulation physique : gravité multi-points, initialisation sphérique, cubique, pyramidale

---

## 🏗️ Architecture

### Composants Principaux

Particule_system/
├── includes/                    # Headers (.hpp)
│   ├── Application.hpp          # Boucle principale
│   ├── AxisGuizmo.hpp			 # Axes de l'espace
│   ├── CameraFps.hpp       	 # Vue FPS
│   ├── CameraOrbit.hpp     	 # Vue orbite
│   ├── Exception.hpp			 # Exceptions custom
│   ├── Global.hpp				 # Global data
│   ├── ImGuiLayer.hpp           # UI debug
│   ├── ParticleSystem.hpp       # Gestion GPU buffers
|   ├── backends				 # Librairie ImGui
|   ├── glad					 # OpenGl loader
│   ├── glm/                     # Librairie mathématiques
|   └── KHR						 # Librairie pour shader
│
├── srcs/                        # Sources (.cpp)
│   ├── main.cpp                 # Entry point
│   ├── Application.cpp
│   ├── AxisGizmo.cpp
│   ├── CameraFps.cpp
│   ├── CameraOrbit.cpp
│   ├── glad.c
│   ├── ImGuiLayer.cpp
│   ├── ParticleSystem.cpp
│   ├── kernels.cl               # KERNELS OPENCL
│   └── imGui/                   # ImGui implementation
│
├── shaders/                     # Shaders GLSL
│   ├── vertex.glsl              # Vertex shader
│   └── fragment.glsl            # Fragment shader
│
├── Makefile                     # Build system
├── docker-compose.yml			 # Docker config
├── dockerfile					 # Launch docker
├── README.md                    # Ce fichier
└── en.subject.pdf               # Sujet du projet

### Pipeline GPU (OpenGl ↔ OpenCl)

┌──────────────────────────────────────────┐
│            GPU Buffers (OpenGL)           │
│                                          │
│   ┌──────────┬──────────┬──────────┐     │
│   │ Position │ Velocity │  Color   │     │
│   └──────────┴──────────┴──────────┘     │
└───────────────┬──────────────────────────┘
                │
        ┌───────▼────────┐
        │     OpenCL     │
        │    Kernels     │
        │ (Gravity Sim)  │
        └───────┬────────┘
                │
┌───────────────▼──────────────────────────┐
│        Synchronisation OpenGL ↔ OpenCL   │
│          (acquire / release)             │
└───────────────┬──────────────────────────┘
                │
        ┌───────▼────────┐
        │ Vertex Shader  │
        └───────┬────────┘
                │
        ┌───────▼────────┐
        │ FragmentShader │
        └───────┬────────┘
                │
        ┌───────▼────────┐
        │    Display     │
        └────────────────┘


---

## 📋 Prérequis

### Système
- **OS** : Linux (testé sur Ubuntu/Debian)
- **Compilateur** : GCC/G++ avec support C++17
- **GPU** : Compatible OpenCL 1.2+

### Dépendances

| Dépendance | Version | Rôle |
|-----------|---------|------|
| **OpenGL** | 3.3+ | Rendu graphique |
| **OpenCL** | 1.2+ | Calcul GPU parallèle |
| **GLFW** | 3.x | Gestion fenêtre/input |
| **GLM** | - | Mathématiques vecteurs/matrices |
| **ImGui** | - | Interface utilisateur |

### 🔨 Compilation
Docker compose is needed
```bash
# The docker installs all the necessary libraries
make			# Launch docker, compile the project and the quit docker
make help		# Print the detail of all rules you need
```

## 🚀 Utilisation

### Lancer l'application
```bash
./Particule_system <nombre_de_particules> <forme_initiale>
```

### Contrôles

| Touche        |	Action        |
|---------------|-----------------|
| W	            |    Avant        |
| A	            |    Gauche       |
| S	            |    Arrière      |
| D	            |    Droite       |
| Q	            |    Haut         |
| E	            |    Bas	      |
| H				|	 Affiche menu |
| F11			|	 Fullscreen	  |
| Souris        |	 Rotation vue |
| Scroll Souris	|    Zoom         |
| Escape        |    Quitter      |

## 📊 Performance

### Optimisations Intégrées

- ✅ GPU compute pour 100k+ particules
- ✅ GL_DYNAMIC_DRAW pour update fréquent
- ✅ Synchronisation GL/CL minimale
- ✅ VBO single-point rendering

## Images

