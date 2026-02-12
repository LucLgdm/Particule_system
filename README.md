# 🚀 Particle System

Un moteur de simulation de particules **haute-performance** utilisant **OpenGL**, **OpenCL** et **ImGui**. Simule des milliers de particules soumises à la gravité avec rendu 3D temps réel et interface de contrôle interactive.

> **Langage** : C++ (88.8%) + C (11.2%)  
> **État** : Stable  
> **Dernière mise à jour** : Janvier 2026

---

## ✨ Caractéristiques

- GPU Compute : calculs physiques parallèles via OpenCL
- Rendu 3D temps réel : pipeline OpenGL 3.3+ avec shaders GLSL
- Caméras multiples : modes FPS et Orbit commutables à la volée
- Interface ImGui : ajout et configuration de points de gravité en temps réel
- Interopérabilité OpenGL / OpenCL : buffers partagés et synchronisation explicite
- Simulation physique : gravité multi-points, initialisation sphérique (Fibonacci sphere)

---

## 🏗️ Architecture

### Composants Principaux

Particule_system/
├── includes/                    # Headers (.hpp)
│   ├── Application.hpp          # Boucle principale
│   ├── ParticleSystem.hpp       # Gestion GPU buffers
│   ├── ImGuiLayer.hpp           # UI debug
│   ├── Camera/
│   │   ├── CameraFps.hpp        # Vue FPS
│   │   └── CameraOrbit.hpp      # Vue orbite
│   ├── exception.hpp            # Exceptions custom
│   └── glm/                     # Librairie mathématiques
│
├── srcs/                        # Sources (.cpp)
│   ├── main.cpp                 # Entry point
│   ├── Application.cpp
│   ├── ParticleSystem.cpp
│   ├── ImGuiLayer.cpp
│   ├── kernels.cl               # KERNELS OPENCL
│   ├── Camera/
│   │   ├── CameraFps.cpp
│   │   └── CameraOrbit.cpp
│   └── imGui/                   # ImGui implementation
│
├── shaders/                     # Shaders GLSL
│   ├── vertex.glsl              # Vertex shader
│   └── fragment.glsl            # Fragment shader
│
├── Makefile                     # Build system
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

### Installation (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  libglfw3-dev libglfw3 \
  libgles2-mesa-dev \
  ocl-icd-libopencl1 opencl-headers \
  mesa-utils

clinfo  # Affiche info GPU et plateforme OpenCL
```

### 🔨 Compilation & Installation
```bash
make          # Compilation avec flags C++17
make clean    # Supprime fichiers objets (.o)
make fclean   # Supprime tout (objets + exécutable)
make re       # Rebuild complet

make val      # Compilation + exécution avec Valgrind (détection fuites)
make help     # Affiche règles disponibles
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

