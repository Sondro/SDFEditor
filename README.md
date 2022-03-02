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

To create the Visual Studio Solution files execute GenerateSolution.bat, this will run the premake tool included in the project. This project has only been tested in Visual Studio 2019 in Windows, but porting to other systems should be easy providing a compiled version of GLFW and premake.

## SDFEditor Quickstart

### Controls

WASD + Right mouse click controls camera movement

F5 Reload the shaders

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


