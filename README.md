# SDFEditor

## Introduction

SDFEditor is a Constructive Solid Geometry editor with a non destructive workflow that uses Sparse Voxel Signed Distance Fields and Raymarching to render the scene.

The project is coded in C++, uses OpenGL 4.5 to evaluate the primitives data in GPU, generate the SDF volume and render it. The interface is also rendered with OpenGL using Dear Imgui.

### SDFEditor Features

- Create Box / Ellipsoid / Torus / Capsule primitives 
- Add / Subtract / Intersect operations
- Primitive blending
- Intuitive Primitive transformation gizmos thanks to ImGuizmo
- Copy / Paste
- Undo / Redo
- Load / Save scene in an open JSON format

### License and Anti-NFT Clausule

SDFEditor has a permissive license that let you use and redistribute SDFEditor in binary or source code if you follow a list of conditions. One of them is an Anti-NFT clausule that explicitly prohibits the usage of this software to produce any kind of content that would be used as NFT or for any other crypto-currency trading. Read the LICENSE.txt file for more information.

## Building

To create the Visual Studio Solution files execute GenerateSolution.bat, this will run the premake tool included in the repo. This project has only been tested in Visual Studio 2019 on a Windows 10 system with an Nvidia GTX 1080Ti card, it is expected to work out of the box with ATI cards too, but report any issue you find please.

Porting to other systems should be easy by providing a compiled version of GLFW and premake, but MacOSX will require some OpenGL 4.5 wrapper or reimplement the renderer and imgui backend to use more than one API, the project is open to collaboration and proposals in that way.

Compatibility with Linux and MacOSX will be added in the future.

## Basic Controls

- W,A,S,D for movement
- Q,E for vertical displacement
- R,T,Y change between Rotation, Translate, Scale modes
- Right mouse click controls camera rotation
- Left mouse click to pick strokes and use gizmos
- F5 Reload the shaders and regenerate the scene
- Ctrl+C / Ctrl+V for Copy / Paste
- Ctrl+Z / Ctrl+Shift+Z for Undo / Redo

## SDFEditor Examples

### Sculpting a face video video
Example video time-lapse creating a basic face with a few primitives.
[![Sculpting a face time-lapse](https://img.youtube.com/vi/LGpUlqWzjd8/0.jpg)](https://www.youtube.com/watch?v=LGpUlqWzjd8)

### Shading
You can use the Shading panel to configure a global material properties, lights and background color.

![Material showdown](/Docs/cars_materials.png)

## Libraries

- GLFW
- Dear ImGui
- glad / KHR
- glm
- dirent
- nlohmann JSON
- stb

## Future plans 
This project is open to collaboration and proposals, so if you want to contribute or just point what could be interesting to implement, you are welcome.

### Important TODOs and known issues
- Fix Camera Rotate input implementation, is interacting with imgui, should be done instead with GLFW callbacks and blocking imgui.
- Fix Raymarcher issues that are causing some artifacts.
- Manage Multiple strokes selection transform, this require some rework on how gizmos are used.

### Looking forward
- Port renderer to other APIs, a vulkan version compatible with may MoltenVK would be interesting.
- Add framebuffers functionality to do offscreen render (for other required TODOs).
- Optimize Raymarching, probably with cone-tracing, but can also be interesting to do checkerobard rendering.
- Optimize stroke evaluation pass, as it is not scaling well with big scenes.
- Scene hirearchy, this also require a transformation stack in strokes evaluation shader.
- Per primitive material would be interesting, but requires extra space in the Atlas buffer to store all those values.
- Adaptive or configurable SDF Lut volume size, now is a fixed size box.
- Temporal Antialiasing.
- Pathtracer, with a different material model.

## Source code notes

### Libraries
- ThirdParty directory contains all the third party libraries used in this project, including the imgui backend. This libraries are compiled as part of the sbx library.
- sbx is a library that carries common utilities from other tool projects. Most of it is removed for now, the intention is move generic functionality from SDFEditor source to sbx.

### SDFEditor modules
- /GPU contains the renderer code that generates SDF data and draws the scene. The resources used from OpenGL 4.5 API are wrapped by a RAII-style set of classes.
- /GUI has most of the imgui calls to draw the user interface, gizmos and manage selection.
- /Tool contains the scene data components where strokes are stored, and the main tool class that controls the camera and shortcuts, calls to renderer, draws the gui, etc.
- /Math & /Utils are generic funcionality used by the tool, candidates to be part of sbx.

### Shaders
Indside Data/Shaders directory you can find all the shaders used by the tool, a further explanation on how they work will be added in the future.

## Contributors

David Gallardo - Author


