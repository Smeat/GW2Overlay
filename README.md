# GW2Overlay

This is aimed to be a (very basic) GW2 overlay which runs natively on Linux as an alternative to GW2TaCo.
The GW2 data is sent via UDP from a python script inside the wine environment, since using mmap with wine is not straightforward.

## Requirements
 * libx11
 * opengl
 * glew
 * boost-program-options
 * openssl
 * SDL2
 * qt5
 * vulkan sdk (including glslc)
 * pthreads

## Compilation
 `mkdir build && cd build && cmake ../src && make`
 You may need to compile an additional binary in `src/helper` with `x86_64-w64-mingw32-g++ mumble_helper.cpp -o mumble.exe -lws2_32` to create the MumbleLink shared memory. It was working for me for days without this program, but suddenly it is required for me. No clue what happened.
 I still rely on the python script to send the mumble data, since the C program is extremely slow. No clue why, and to be honest, I don't want to dig deeper into the Windows API.
 The helper program gets automatically started if it exist. Currently too lazy to add it to the CMake file.

 I'd still love to have a direct memory mapping, without having to use sockets though


## Usage
 * Start GW2
 * Launch the `launch.sh --xml-folder <path to the taco xmls to load> --vulkan` script with the path to the `GW2Overlay` binary
   * All the required resources are expected to be in the current working directory

If you start the script (specifically the python part in wine) before GW2, reading the memory is broken and GW2 needs to be restarted (maybe even the wineserver). No clue why this happens.
GW2 itself needs to be started and at least on the character select screen.
The launch script tries to wait for GW2 to start completely, but it not working reliably.

### Status
 * Current status: It is usable, but the UI is still lacking (Just a simple window to activate/deactivate POIs)
 * Can load and display GW2TaCo XML POIs
 * Tested on Arch with i3wm and picom
 * Position is currently hardcoded for my setup
 * OpenGL and Vulkan renderers both working
   * CPU usage with Vulkan is pretty much non existent. Most is just calculating the view matrix

### TODO
 * Some kind of GUI for settings etc. (Very basic one is implemented)
   * Boss timer
   * API key (daily)
 * Vulkan specific
   * Depth buffer
   * Draw everything in a single cmd (might also fix the bug after creating the queue)
   * Proper color blending
 
### BUGS
 * Vulkan specific
   * For the first seconds after creating the new command buffers (when switching the displayed POIs) the overlay lags behind. After that it works perfectly
   * POI objects behind transparency look weird
  
 ![screenshot](https://user-images.githubusercontent.com/1054449/90751525-89bd8880-e2d6-11ea-9985-2ffa561d9049.png)
