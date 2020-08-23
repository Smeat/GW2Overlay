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

## Usage
 * Start GW2
 * Launch the `launch.sh --xml-folder <path to the taco xmls to load>` script with the path to the `GW2Overlay` binary
   * All the required resources are expected to be in the current working directory

If you start the script (specifically the python part in wine) before GW2, reading the memory is broken and GW2 needs to be restarted (maybe even the wineserver). No clue why this happens.
GW2 itself needs to be started and at least on the character select screen.
The launch script tries to wait for GW2 to start completely, but it not working reliably.

### Status
 * Current status: Very early PoC
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
