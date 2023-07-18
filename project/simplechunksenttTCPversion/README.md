# Stable Standalone Client for mimosdono.xyz

## Description:

* Using the common GLFW3, GLEW, and GLM libraries to make a simple, fast stand-alone PC client for mimosdono.xyz, a web-based voxel game written in JavaScript with Three, React, Socketio and Firebase.

## Technologies used:

* GLFW, GLEW, GLM
* Folly hashmap for storing world voxel data.
* vcpkg for managing C++ libraries

## Installation:

### Vcpkg and dependencies

1. Make sure you have vcpkg installed and set up, as explained here: https://vcpkg.io/en/getting-started.html
2. For 64 bit Windows, run `vcpkg install glfw3:windows-x64`, or `./vcpkg.exe install glfw3:windows-x64` in the vcpkg directory if you do not have vcpkg.exe available on your PATH. If not on Windows x64, replace `:windows-x64` with the correct triplet for your system. Follow this pattern for installing the following packages:
  * `vcpkg install glew:windows-x64`
  * `vcpkg install glm:windows-x64`
  * `vcpkg install folly:windows-x64`

### After Vcpkg and dependencies are installed

1. Clone this repository
2. Open Visual Studio
3. Go to File -> Open -> Project/Solution
4. Browse to this repository's directory containing the .vcxproj file
5. Select the .vcxproj file and click "Open"
6. Visual Studio should automatically load the project and its associated files.

## License:

Copyright (c) 2023 Jackson Levine

This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.

This notice may not be removed or altered from any source distribution.