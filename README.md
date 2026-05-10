# Inspiral

A custom graphics engine made for inspired individuals.

![screenshot](screenshots/f1.png)
![screenshot](screenshots/chess.png)
![screenshot](screenshots/pawn-wireframe.png)

## Features
- GLTF/GLB scene loading
- OBJ import
- Scene viewer
- Blinn-Phong lighting with multiple light sources
- and even more to come!

## Building
```bash
cmake -S . -B build
cmake --build build --target engine
```

## Dependencies (are downloaded automatically by cmake script)
- OpenGL 3.3+
- GLFW
- GLM
- ImGui
- tinygltf
  
