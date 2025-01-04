# Duck with water simulation

https://github.com/user-attachments/assets/4b0f687f-7954-4b4c-8f58-837c8ca44590

## About project 

This is one of the projects created during *"Computer Graphics 2"* course, specialty **CAD/CAM systems design** of degree in *Computer Science and Information Systems* at *Warsaw University of Technology*. 

Project features: 

- The duck moves along segments of B-spline.
- Custom procedurally generated billboards are used to represent point lights on the scene.
- The scene is lit using the Phong model.
- Compute shaders are used for water simulation and respective computation of water surface normal vectors.
- Snell's window effect when looking from under the water. 

## Navigation 

<kbd>LMB</kbd> - camera rotation

<kbd>RMB</kbd> - camera zoom in/out

## Compilation 

The project is written in C++ (OpenGL) using CMake as a build system. It has two configuration CMake options: 

| Option | Description | 
| :---:         |     :---      |
| `DUCK_EMBEDDED_SHADERS`   | If `ON` shader code will be embedded into a resulting app's binary. If `OFF` shader code will be read from `shaders` directory, so a binary has to be run from repository root directory.      |
| `DUCK_SHOW_DEBUG_CONSOLE` | If `ON` - beside app's window - a debug console will be shown on app start up. If `OFF` only app's window will be shown.        |

### Windows 

All subsequent commands should be called from Windows Power Shell.

```
git clone --recurse-submodules -j8 https://github.com/Tolisz/ComputerGraphics_Duck.git
cd ComputerGraphics_Duck
cmake -S . -B build -DDUCK_EMBEDDED_SHADERS=ON -DDUCK_SHOW_DEBUG_CONSOLE=OFF
cmake --build build --config Release
```

```
.\build\Release\duck.exe
```

### Linux

All subsequent commands should be called from terminal. Compilation was checked using g++ (version 11.4.0) and clang++ (version 14.0.0) compilers.

```
git clone --recurse-submodules -j8 https://github.com/Tolisz/ComputerGraphics_Duck.git
cd ComputerGraphics_Duck
cmake -S . -B build -DDUCK_EMBEDDED_SHADERS=ON -DDUCK_SHOW_DEBUG_CONSOLE=OFF
cmake --build build --config Release -j 26
```

```
./build/duck
```

## Used Libraries

Libraries `GLFW`, `GLM`, `STB` and `ImGui` are used as submodules located in `externals` directory. `GLAD` is used as a generated file from [glad](https://glad.dav1d.de/) web page. 

| Library | Licence |
| :---:   | :---:   |
| [GLFW](https://github.com/glfw/glfw) | [Zlib](https://github.com/glfw/glfw?tab=Zlib-1-ov-file#readme) |
| [GLM](https://github.com/g-truc/glm) | [Modified MIT](https://github.com/g-truc/glm?tab=License-1-ov-file#readme)|
| [ImGui](https://github.com/ocornut/imgui) | [MIT](https://github.com/ocornut/imgui?tab=MIT-1-ov-file#readme)| 
| [GLAD](https://github.com/Dav1dde/glad) | [MIT](https://github.com/Dav1dde/glad?tab=License-1-ov-file#readme) | 
| [STB](https://github.com/nothings/stb) | [Modified MIT](https://github.com/nothings/stb?tab=License-1-ov-file#readme) | 
