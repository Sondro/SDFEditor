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

To create the Visual Studio Solution files execute GenerateSolution.bat, this will run the premake tool included in the project. This project has only been tested in Visual Studio 2019 on a Windows 10 system with an Nvidia GTX 1080Ti card, but porting to other systems should be easy by providing a compiled version of GLFW and premake. 

Compatibility with Linux and MacOSX will be added sooner or later to this repo.

## Basic Controls

* WASD for movement
* Right mouse click controls camera movement
* Left mouse clic pick scene objects and use gizmos.
* F5 Reload the shaders

## SDFEditor Showdown

### Sculpting a face time-lapse

[![Sculpting a face time-lapse](https://img.youtube.com/vi/LGpUlqWzjd8/0.jpg)](https://www.youtube.com/watch?v=LGpUlqWzjd8)

### Materials

![Material showdown](/Docs/cars_materials.png)

## Libraries

- GLFW
- Dear ImGui
- glad / KHR
- glm
- dirent
- nlohmann JSON
- stb

## Contributors

David Gallardo - Author


