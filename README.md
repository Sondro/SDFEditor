# SDFEditor

## Introduction

SDFEditor is a Constructive Solid Geometry editor with a non destructive workflow that uses Sparse Voxel Signed Distance Fields and Raymarching to render the scene.

The project is coded in C++, uses OpenGL 4.5 to interpret the primitives data in GPU, generate the SDF volume and render it. The interface is also rendered with OpenGL using Dear Imgui.

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

## Important TODOs

## Source code notes
- ThirdParty directory contains all the third party libraries used in the project, including the imgui backend.
- sbx is a library I use for other tool projects with common utilities, I removed most of it for now, the intention is move generic functionality from SDFEditor source to sbx.

### SDFEditor modules
- GPU contains the renderer code, with a RAII wrapper for some OpenGL 4.5 resources.
- GUI has most of the imgui calls to draw the user interface, gizmos and manage selection.
- Tool contains the scene data components where strokes are stored, and the main tool class that controls the camera and shortcuts, calls to renderer, draws the gui, etc.
- Math & Utils are generic funcionality used by the tool, candidates to be part of sbx.

### Shaders
Indside Data/Shaders directory you can find all the shaders used by the tool, a further explanation on how they work will be added in the future.

## Contributors

David Gallardo - Author


