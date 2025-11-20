# ⚽ Football Training Facility (OpenGL Project)

This project implements a **3D football training simulator** developed using **C++** and **OpenGL**.  
It renders an interactive football training ground with realistic lighting, shading, and object manipulation, providing users with a fully navigable and dynamic virtual environment.

---

## 🚀 Features

- 🌳 **Fully Modeled 3D Environment** – Football pitch, trees, player, bus, and stadium surroundings  
- 💡 **Real-Time Lighting Control** – Rotate and adjust the global light source dynamically  
- 🧭 **Camera Navigation** – Move freely using keyboard input to explore the environment  
- 🧱 **Multiple Rendering Modes** – Toggle between wireframe, point, and solid fill views  
- 🔦 **Point Light Effects** – Activate localized lighting directly from the grass area  
- 🎮 **Scene Preview Mode** – Switch perspectives for alternate viewing angles  
- ⚙️ **Optimized Performance** – Smooth frame rates and efficient use of shaders and buffers  

---

## 🧰 Technologies Used

- **C++17** – Core programming language  
- **OpenGL 4.x** – Graphics rendering API  
- **GLM** – Mathematics library for 3D transformations  
- **GLFW / GLEW** – Window, input, and OpenGL context management  
- **OBJ Loader (tinyobjloader)** – For importing 3D models  
- **stb_image** – Image loading for textures and skyboxes  

---

## 🖼️ Application Overview

The project renders a **football training scene** featuring multiple objects and realistic lighting techniques:

- 🏟️ **Football Pitch** – Complete with lines, goalposts, and detailed textures  
- 🚌 **Bus Station & Team Bus** – Modeled to add realism to the environment  
- 🌲 **Surrounding Trees** – Animated elements for visual appeal  
- 🧍‍♂️ **Football Player** – Practicing shots on the field  
- ☀️ **Global Light Source** – Adjustable lighting for dynamic illumination  

The scene supports user interactions such as moving the camera, toggling views, and adjusting light conditions.

---

## 🕹️ Controls

| Action | Key |
|--------|-----|
| Move camera | **W / A / S / D** |
| Change render mode | **7 / 8 / 9** |
| Rotate global light | **K / L** |
| Enable/disable scene preview | **1 / 2** |
| Toggle point light | **3 / 4** |

---

## 📂 Project Structure

- `Camera.*` – Handles camera movement and view matrix logic  
- `Mesh.*` – Manages 3D geometry and vertex data  
- `Model3D.*` – Loads and renders 3D models (.obj)  
- `Shader.*` – Manages GLSL shaders for lighting and materials  
- `SkyBox.*` – Implements background environment cube mapping  
- `objects/` – Contains 3D object files (OBJ format)  
- `shaders/` – GLSL shader programs for lighting, materials, and rendering  
- `textures/` – Image files for surface textures and skybox layers  
- `Project.vcxproj` – Visual Studio project configuration  

---

## ▶️ How to Build and Run

### 🧩 Requirements
- **Visual Studio 2019/2022** (with Desktop development with C++)  
- **OpenGL-compatible GPU**  
- **GLFW**, **GLEW**, and **GLM** libraries (included or linked in the project)

### 🧠 Steps
1. Open the solution file `Project.vcxproj` in **Visual Studio**.  
2. Ensure dependencies (GLFW, GLEW, GLM) are properly configured.  
3. Build the project in **Release** or **Debug** mode.  
4. Run the executable from Visual Studio or from the `Executabil/` folder.  
5. Use the keyboard controls to explore the scene.

---

## 🔮 Future Improvements

Planned enhancements for future versions include:

- ⚽ **Physics Simulation** – Realistic ball dynamics and collision detection  
- 🤖 **AI Opponents** – Simulated players for training drills  
- 🌧️ **Weather Effects** – Rain, fog, and environmental lighting changes  
- 🧑‍🏫 **Virtual Coach** – Provides tips and training feedback  
- 🧍 **Character Customization** – Adjustable player appearance and stats  
- 🎧 **Audio Integration** – Ambient sounds and commentary  
- 🕹️ **VR or Motion Capture Support** – For immersive training experiences  

---

This project was completed as part of my studies at the Technical University of Cluj-Napoca (UTCN) 🎓.
