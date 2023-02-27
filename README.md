# SVG Rasterizer

This is a small SVG software rasterizer based on the assignment from CMU's Computer Graphics course, DrawSVG. 

Features implemented by me:

1. Hardware rasterization
2. Software line rasterization
3. Software triangle rasterization
4. Multithreaded supersampling
5. Modeling and viewing transforms

### Build Instructions

In order to ease the process of running on different platforms, we will be using [CMake](http://www.cmake.org/) for our assignments. You will need a CMake installation of version 2.8+ to build the code for this assignment. It should also be relatively easy to build the assignment and work locally on Windows, OSX or 64-bit version of Linux. Building on ARM (e.g. Raspberry Pi, some Chromebooks) is currently not supported.

#### VSCode Build Instructions (All Platforms)

We recommend using [Visual Studio Code](https://code.visualstudio.com/download) on all platforms. Once you install CMake and VSCode, you will also need to install the C/C++ extension within VSCode.

The build and launch data is contained within the `.vscode` directory. Select the folder for your compiler and move `launch.json` and `tasks.json` up one level into the `.vscode` directory. To set commandline arguments, go to `launch.json` and add them within `args`. For example, to run the program with test 1, you would add `./basic/test1.svg`. You can build using <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>B</kbd> and debug using <kbd>F5</kbd>. If you feel that your program is running slowly, you can also change the build mode to `Release` from `Debug`.

Commonly used Hotkeys:

- <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>B</kbd> - Build
- <kbd>F5</kbd> - Debug
  - <kbd>F9</kbd> - Toggle Breakpoint
  - <kbd>F10</kbd> - Step Over
  - <kbd>F11</kbd> - Step Into
  - <kbd>Shift</kbd>+<kbd>F5</kbd> - Stop
- <kbd>Ctrl</kbd>+<kbd>P</kbd> - Search for file
- <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>P</kbd> - Run Command
- Right click to go to definition/declaration/references

#### OS X/Linux Build Instructions

If you are working on OS X and do not have CMake installed, we recommend installing it through [Homebrew](http://brew.sh/): `$ brew install cmake`. You may also need the freetype package `$ brew install freetype`.

If you are working on Linux, you should be able to install dependencies with your system's package manager as needed (you may need cmake and freetype, and possibly others).

To build your code for this assignment:

```
$ cd DrawSVG && mkdir build && cd build
$ cmake ..
$ make
```

These steps (1) create an out-of-source build directory, (2) configure the project using CMake, and (3) compile the project. If all goes well, you should see an executable `drawsvg` in the build directory. As you work, simply typing `make` in the build directory will recompile the project.

#### Windows Build Instructions using Visual Studio

We have a beta build support for Windows systems. You need to install the latest version of [CMake](http://www.cmake.org/) and install [Visual Studio Community](https://visualstudio.microsoft.com/vs/). After installing these programs, you can run `runcmake_win.bat` by double-clicking on it. This should create a `build` directory with a Visual Studio solution file in it named `drawsvg.sln`. You can double-click this file to open the solution in Visual Studio.

If you plan on using Visual Studio to debug your program, you can change `drawsvg` project in the Solution Explorer as the startup project by right-clicking on it and selecting `Set as StartUp Project`. You can also set the commandline arguments to the project by right-clicking `drawsvg` project again, selecting `Properties`, going into the `Debugging` tab, and setting the value in `Command Arguments`. If you want to run the program with the basic svg folder, you can set this command argument to `../../svg/basic`. After setting all these, you can hit F5 to build your program and run it with the debugger.

If you feel that your program is running slowly, you can also change the build mode to `Release` from `Debug` by clicking the Solution Configurations drop down menu on the top menu bar. Note that you will have to set `Command Arguments` again if you change the build mode.

#### Windows Build Instructions Using CLion

(tested on CLion 2018.3)

Open CLion, then do `File -> Import Project..`

In the popped out window, find and select the project folder `...\DrawSVG`, click OK, click Open Existing Project, then select New Window

Make sure the drop down menu on top right has drawsvg selected (it should say `drawsvg | Debug`). Then open the drop down menu again and go to Edit Configurations..

Fill in Program arguments, say, `./svg/basic`, then click Apply and close the popup

Now you should be able to click on the green run button on top right to run the project.

### Using the Mini-SVG Viewer App

When you have successfully built your code, you will get an executable named **drawsvg**. The **drawsvg** executable takes exactly one argument from the command line. You may load a single SVG file by specifying its path. For example, to load the example file `svg/basic/test1.svg` :

```
./drawsvg ../svg/basic/test1.svg
```

When you first run the application, you will see a picture of a flower made of a bunch of blue points. The starter code that you must modify is drawing these points. Now press the R key to toggle display to the staff's reference solution to this assignment. You'll see that the reference differs from "your solution" in that it has a black rectangle around the flower. (This is because you haven't implemented line drawing yet!)

While looking at the reference solution, hold down your primary mouse button (left button) and drag the cursor to pan the view. You can also use scroll wheel to zoom the view. (You can always hit SPACE to reset the viewport to the default view conditions). You can also compare the output of your implementation with that of the reference implementation. To toggle the diff view, press D. We have also provided you with a "pixel-inspector" view to examine pixel-level details of the currently displayed implementation more clearly. The pixel inspector is toggled with the Z key.

For convenience, `drawsvg` can also accept a path to a directory that contains multiple SVG files. To load files from `svg/basic`:

```
./drawsvg ../svg/basic
```

The application will load up to nine files from that path and each file will be loaded into a tab. You can switch to a specific tab using keys 1 through 9.

### Summary of Viewer Controls

A table of all the keyboard controls in the **draw** application is provided below.

| Command                                           |  Key  |
| ------------------------------------------------- | :---: |
| Go to tab                                         | 1 ~ 9 |
| Switch to hw renderer                             |   H   |
| Switch to sw renderer                             |   S   |
| Toggle sw renderer impl (student soln/ref soln)   |   R   |
| Increase samples per pixel                        |   =   |
| Decrease samples per pixel                        |   -   |
| Toggle text overlay                               |   `   |
| Toggle pixel inspector view                       |   Z   |
| Toggle image diff view                            |   D   |
| Reset viewport to default position                | SPACE |

Other controls:

- Panning the view: click and drag the cursor
- Zooming in and out: scroll wheel (typically a two-finger drag on a trackpad)
