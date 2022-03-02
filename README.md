# SDFEditor

## Introduction

SDFEditor is a Constructive Solid Geometry editor that uses Sparse Voxel Signed Distance Fields and Raymarching to render the scene.

The project is coded in C++, uses OpenGL 4.5 to interpret the primitives data in GPU, generate the SDF data and draw it.

### SDFEditor Features

- Create Box / Ellipsoid / Torus / Capsule primitives 
- Add / Subtract / Intersect operations
- Primitive blending
- Intuitive Primitive transformation gizmos thanks to ImGuizmo
- Copy / Paste
- Undo / Redo
- Load / Save scene in an open JSON format

## Building

To create the Visual Studio Solution files execute GenerateSolution.bat, this will run the premake tool included in the project. This project has only been tested in Visual Studio 2019 in Windows, but porting to other systems should be easy providing a compiled version of GLFW and premake.

## SDFEditor Quickstart

### Controls

WASD + Right mouse lickc controls camera movement

F5 Reloadthe shaders

## Libraries

- GLFW
- Dear ImGui
- glad / KHR
- glm
- dirent
- nlohmann JSON
- stb

## Source Code Description


