# Elementary Visualizer

Simple visualization library.

## Building

In the project root directory, run the following commands to build the project.

```
cmake -S . -B build
cmake --build build
```

## Running tests and automatic reformatting

To run tests, run the following command.
```
ctest --test-dir build
```

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
* Use the `this->` strictly when referring to member function or variable.
* Do not check for the validity of (smart) pointers of the type
  `Expected<P, E>`, because if this returns successfully,
  assume that the pointer is also valid.
  (This also implies everything which returns this type
  should make sure that the (smart) pointer is valid in this case.)
* The validity of (smart) pointers for private member variables
  should not be checked if they are expected
  to be setup correctly inside the class.
* (Smart) pointers coming from outside (non-member)
  should be always checked.
* Use constants whenever possible.
* Everything should run in a single thread.
  This is a limitation of GLFW and OpenGL.

## Todo

* Add build requirements in readme.
* Features.
  * Lines.
    * Similar to linesegment, but it's made up of lots
      of linesegments and their connections, so that the line
      is more or less continuous.
    * Texturing along the line; dashed lines.
  * Surface plot.
    * The x, y, z coordinates are specified
      in a 2 dimensional data,
      and this defines the whole surface
      (the index is the u, v coordinate).
    * Simple Phong reflection model.
    * Two modes: smooth and flat.
      * Smooth mode: colors are defined at vertices,
        normals are interpolated between vertices.
      * Flat mode: colors are defined inside rectangles,
        normals are not interpolated,
        they are constant for a half rectangle (triangle).
    * Possibly texturing.
  * Rendering the rendered scene to `Image`.
  * Rendering the rendered scene to `Video`.
  * Text.
    * Basic text.
    * Latex equations.
  * Volumetric plot.
* Error handling.
  * Implement `Error` class correctly, and populate
    it's data correctly when it is created anywhere.
  * Functions which return `void` should return
    a value which indicates whether they succeed,
    or fail if the function can fail internally.
    The reason for failure should be returned.
  * Use `glCheckFramebufferStatus` to check framebuffer status
    when creating/using framebuffers.
* Tests.
  * Add tests for public class copy/move, and check their validity.
  * Check not only `add_visual` but also `remove_visual` in the tests.
* Documentation; possibly use doxygen.
* Use testing framework; possibly GoogleTest or Catch2.

## Licensing

This software is distributed under the GNU General Public License (GPL) version 3. You can find the full text of the license in the [LICENSE](LICENSE.txt) file.

### Other Licensing Options

Upon request, alternative licensing options may be available for this software.

