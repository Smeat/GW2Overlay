# GW2Overlay

This is aimed to be a (very basic) GW2 overlay which runs natively on Linux as an alternative to GW2Taco.
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

Python for Windows installed in `${WINEPREFIX}/drive_c/Program Files/Python38/python.exe`

## Compilation
 `mkdir build && cd build && cmake ../src && make`


## Usage
 * Start GW2
 * Start the overlay with `./GW2Overlay --xml-folder <path to your poi folder> --vulkan`
   * Open the options menu and select the location of the `mumble.py` helper script (one time)
   * All the required resources are expected to be in the current working directory

### Status
 * Current status: It is usable, but the UI is still lacking (Just a simple window to activate/deactivate POIs)
 * Can load and display GW2Taco XML POIs
 * Tested on Arch with i3wm and picom
 * Position is currently hardcoded for my setup
 * OpenGL and Vulkan renderers both working
   * CPU usage with Vulkan is pretty much non existent. Most is just calculating the view matrix
   * Seeing as a very bad Vulkan implementation, is much faster than the OpenGL version, the OpenGL version won't get any further updates

### TODO
 * Some kind of GUI for settings etc. (Very basic one is implemented)
   * Boss timer/reminder
   * daily, killed bosses
   * Make the GUI GW2 themed
   * Overlay manager to create Overlays with boss timers and other stuff
 * Icons on Map and Minimap (a basic implementation is working, but only works in a few areas)
 * Vulkan specific
   * Depth buffer
   * Draw everything in a single cmd (might also fix the bug after creating the queue)
   * Proper color blending
 
### BUGS
 * Vulkan specific
   * For the first seconds after creating the new command buffers (when switching the displayed POIs) the overlay lags behind. After that it works perfectly
   * POI objects behind transparency look weird
   * If you encounter any visual related bugs/crashes when using vulkan, please start the program with `--validation` (for the very slim chance someone other than me is using this program)
  
 ![screenshot](https://user-images.githubusercontent.com/1054449/90751525-89bd8880-e2d6-11ea-9985-2ffa561d9049.png)
