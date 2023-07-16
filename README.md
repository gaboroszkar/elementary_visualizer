# Elementary Visualizer

Simple visualization library.

## Building

In the project root directory, run the following commands to build the project.

```
cmake -S . -B build
cmake --build build
```

## Running automatic reformatting

To automatically reformat the code, run the following command.
```
cmake --build build --target clangformat
```

## Credits

This project makes use of the following open-source software.

- [expected](https://github.com/TartanLlama/expected.git): Single header implementation of `std::expected` with functional-style extensions.
- [GLFW](https://www.glfw.org/): GLFW is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan application development.
- [glad](https://github.com/Dav1dde/glad.git): Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specifications for multiple languages.
- [glm](https://github.com/g-truc/glm.git): Header only C++ mathematics library for graphics software.

## Development guidelines

* Before each commit, use the automatic code formatter.

## Licensing

This software is distributed under the GNU General Public License (GPL) version 3. You can find the full text of the license in the [LICENSE](LICENSE.txt) file.

### Other Licensing Options

Upon request, alternative licensing options may be available for this software.

